/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "robot.h"
#include <fstream>
#include "bosdyn/client/auth/auth_client.h"
#include "bosdyn/client/directory/directory_client.h"
#include "bosdyn/client/directory_registration/directory_registration_client.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"
#include "bosdyn/client/estop/estop_client.h"
#include "bosdyn/client/payload_registration/payload_registration_client.h"
#include "bosdyn/client/power/power_client.h"
#include "bosdyn/client/power/power_client_helper.h"
#include "bosdyn/client/robot_command/robot_command_client.h"
#include "bosdyn/client/robot_id/robot_id_client.h"
#include "bosdyn/client/robot_state/robot_state_client.h"
#include "bosdyn/client/service_client/channel.h"


namespace bosdyn {

namespace client {

Robot::Robot(const std::string& client_name, const bool bypass_proxy,
             ::bosdyn::common::Duration timeout)
    : m_token_manager(nullptr),
      m_bootstrap_authorities_by_name(
          {{DirectoryClient::GetDefaultServiceName(),
            DirectoryClient::GetDefaultServiceAuthority()},
           {DirectoryRegistrationClient::GetDefaultServiceName(),
            DirectoryRegistrationClient::GetDefaultServiceAuthority()},
           {RobotIdClient::GetDefaultServiceName(), RobotIdClient::GetDefaultServiceAuthority()},
           {AuthClient::GetDefaultServiceName(), AuthClient::GetDefaultServiceAuthority()},
           {PayloadRegistrationClient::GetDefaultServiceName(),
            PayloadRegistrationClient::GetDefaultServiceAuthority()}}),
      m_bypass_proxy(bypass_proxy),
      m_bootstrap_endpoints_by_name(
          
          {
          {DirectoryClient::GetDefaultServiceName(),
            {"127.0.0.1", 65033, DirectoryClient::GetServiceType()}},
           {DirectoryRegistrationClient::GetDefaultServiceName(),
            {"127.0.0.1", 65033, DirectoryRegistrationClient::GetServiceType()}}}),
      m_secure_channel_port(443) {
    m_RPC_parameters.timeout = timeout;
    TokenCache* raw_token_cache = new TokenCache();
    m_token_cache = std::unique_ptr<TokenCache>(raw_token_cache);

    m_lease_wallet = std::shared_ptr<LeaseWallet>(new LeaseWallet(client_name));
}

Robot::~Robot() {
    StopTimeSync();
    // Release TokenManager first to join its thread before shutting down the MessagePump.
    m_token_manager.reset(nullptr);
    // Ensure the message pump is stopped before deleting any clients.
    if (m_default_message_pump) {
        m_default_message_pump->RequestShutdown();
        m_default_message_pump.reset();
    }
}

void Robot::SetNetworkAddress(const std::string& network_address) {
    m_network_address = network_address;
}

void Robot::SetGlobalProcessorChains(const RequestProcessorChain& request_processor_chain,
                                     const ResponseProcessorChain& response_processor_chain) {
    m_request_processor_chain = request_processor_chain;
    m_response_processor_chain = response_processor_chain;
}

::bosdyn::common::Status Robot::Authenticate(const std::string& username,
                                             const std::string& password) {
    Result<AuthClient*> auth_service_client_result = EnsureServiceClient<AuthClient>();
    if (!auth_service_client_result) {
        return auth_service_client_result.status;
    }

    AuthResultType result = auth_service_client_result.response->GetAuthToken(username, password);
    if (!result.status) {
        return result.status;
    }

    UpdateUserToken(result.response.token(), username);
    return ::bosdyn::common::Status(SDKErrorCode::Success);
}

::bosdyn::common::Status Robot::AuthenticateWithToken(const std::string& token) {
    Result<AuthClient*> auth_service_client_result = EnsureServiceClient<AuthClient>();
    if (!auth_service_client_result) {
        return auth_service_client_result.status;
    }

    auto result = auth_service_client_result.response->GetAuthToken(token);
    if (!result) {
        return result.status;
    }

    UpdateUserToken(result.response.token());
    return ::bosdyn::common::Status(SDKErrorCode::Success);
}

::bosdyn::common::Status Robot::SetupClient(ServiceClient* service_client,
                                            const std::string& service_name,
                                            const std::string& service_type,
                                            std::shared_ptr<grpc::ChannelInterface> channel,
                                            std::shared_ptr<MessagePump> message_pump) {
    // Check that the ServiceClient was provided.
    BOSDYN_ASSERT_PRECONDITION(service_client != nullptr, "Cannot setup null service client.");

    // Set the RPC parameters (logging, timeouts) for the client to the default params.
    service_client->SetRPCParameters(m_RPC_parameters);

    // External
    if (!channel) {
        Result<std::shared_ptr<grpc::ChannelInterface>> channel_result =
            EnsureChannel(service_name, service_type);
        if (!channel_result.status) {
            return channel_result.status;
        }
        channel = channel_result.response;
    }

    // Make sure there is a MessagePump to use. If not, use the default message pump if available.
    // Set this as part of the client.
    if (!message_pump) {
        if (!m_default_message_pump) {
            return ::bosdyn::common::Status(
                SDKErrorCode::GenericSDKError,
                "No available MessagePump to use in EnsureServiceClient");
        }
        message_pump = m_default_message_pump;
    }
    service_client->SetMessagePump(message_pump);

    // Update the service client using the robot's processors and lease wallet.
    service_client->UpdateServiceFrom(m_request_processor_chain, m_response_processor_chain,
                                      m_lease_wallet);


    service_client->SetComms(channel);

    return ::bosdyn::common::Status(ClientCreationErrorCode::Success);
}

void Robot::AddCustomRequestProcessor(const std::shared_ptr<RequestProcessor>& request_processor) {
    m_request_processor_chain.AppendProcessor(request_processor);
}

void Robot::AddCustomResponseProcessor(
    const std::shared_ptr<ResponseProcessor>& response_processor) {
    m_response_processor_chain.AppendProcessor(response_processor);
}

Result<std::shared_ptr<grpc::ChannelInterface>> Robot::EnsureChannel(
    const std::string& service_name, const std::string& service_type) {
    if (m_bypass_proxy) {
        // Make sure we have the endpoint for this service first.
        std::map<std::string, Endpoint>::const_iterator endpoint_iter =
            m_bootstrap_endpoints_by_name.find(service_name);
        if (endpoint_iter == m_bootstrap_endpoints_by_name.end()) {
            endpoint_iter = m_endpoints_by_name.find(service_name);
            if (endpoint_iter == m_endpoints_by_name.end()) {
                // Sync with Directory to get the endpoints.
                Result<std::vector<::bosdyn::api::ServiceEntry>> services_result = ListServices();
                if (!services_result.status) {
                    return {services_result.status, nullptr};
                }

                endpoint_iter = m_endpoints_by_name.find(service_name);
                if (endpoint_iter == m_endpoints_by_name.end()) {
                    return {::bosdyn::common::Status(ClientCreationErrorCode::UnregisteredService,
                                                     "Could not find endpoint for " + service_name),
                            nullptr};
                }
            }
        }
        // Using the 'Endpoint' struct, verify that the service type for this specific
        // service name matches the expected service type.
        if ((*endpoint_iter).second.service_type != service_type) {
            return {::bosdyn::common::Status(ClientCreationErrorCode::IncorrectServiceType,
                                             "Endpoint for " + service_name + " has service type " +
                                                 (*endpoint_iter).second.service_type +
                                                 ". The expected type is: " + service_type),
                    nullptr};
        }

        return EnsureInsecureChannel((*endpoint_iter).second);
    }

    // For external secure channels, we need to get authority to find the right channel.
    std::map<std::string, std::string>::const_iterator authority_iter =
        m_bootstrap_authorities_by_name.find(service_name);
    if (authority_iter == m_bootstrap_authorities_by_name.end()) {
        authority_iter = m_authorities_by_name.find(service_name);
        if (authority_iter == m_authorities_by_name.end()) {
            // Sync with Directory to get the authorities.
            Result<std::vector<::bosdyn::api::ServiceEntry>> services_result = ListServices();
            if (!services_result.status) {
                return {services_result.status, nullptr};
            }

            authority_iter = m_authorities_by_name.find(service_name);
            if (authority_iter == m_authorities_by_name.end()) {
                return {::bosdyn::common::Status(SDKErrorCode::GenericSDKError,
                                                 "Could not find authority for " + service_name),
                        nullptr};
            }
        }
    }
    return EnsureSecureChannel((*authority_iter).second);
}

Result<std::shared_ptr<grpc::ChannelInterface>> Robot::EnsureSecureChannel(
    const std::string& authority) {
    std::map<std::string, std::shared_ptr<grpc::ChannelInterface>>::iterator iter =
        m_channels.find(authority);
    if (iter != m_channels.end()) {
        return {::bosdyn::common::Status(SDKErrorCode::Success), (*iter).second};
    }

    // Secure Channel doesn't exist, so create it.
    std::shared_ptr<grpc::ChannelCredentials> creds =
        Channel::CreateSecureChannelCreds(m_cert, std::bind(&Robot::GetUserToken, this));

    std::shared_ptr<grpc::ChannelInterface> channel =
        Channel::CreateSecureChannel(m_network_address, m_secure_channel_port, creds, authority);

    m_channels[authority] = channel;
    return {::bosdyn::common::Status(SDKErrorCode::Success), channel};
}

Result<std::shared_ptr<grpc::ChannelInterface>> Robot::EnsureInsecureChannel(
    const Endpoint& endpoint) {
    std::string endpoint_str = endpoint.GetEndpointString();

    std::map<std::string, std::shared_ptr<grpc::ChannelInterface>>::iterator channel_iter =
        m_channels.find(endpoint_str);
    if (channel_iter != m_channels.end()) {
        return {::bosdyn::common::Status(SDKErrorCode::Success), (*channel_iter).second};
    }

    // Insecure Channel doesn't exist, so create it.
    std::shared_ptr<grpc::ChannelInterface> channel =
        Channel::CreateInsecureChannel(endpoint.host_ip, endpoint.port);

    m_channels[endpoint_str] = channel;
    return {::bosdyn::common::Status(SDKErrorCode::Success), channel};
}

void Robot::UpdateTokenCache(const std::string& username) {
    if (!m_token_manager) {
        TokenManager* raw_token_manager = new TokenManager(this);
        m_token_manager = std::unique_ptr<TokenManager>(raw_token_manager);
    }

    if (!username.empty()) m_current_user = username;

    if (!m_current_user.empty()) {
        std::string key = GetTokenId(m_current_user);
        m_token_cache->write(key, m_user_token);
    }
}

::bosdyn::common::Status Robot::SetupTokenCache(std::unique_ptr<TokenCache> token_cache,
                                                const std::string& unique_id) {
    if (!unique_id.empty()) m_serial_number = unique_id;
    if (m_serial_number.empty()) {
        Result<::bosdyn::api::RobotIdResponse> result = GetId();
        if (!result.status) {
            return result.status;
        }
        m_serial_number = result.response.robot_id().serial_number();
    }

    if (token_cache) m_token_cache = std::move(token_cache);

    return ::bosdyn::common::Status(SDKErrorCode::Success);
}

Result<::bosdyn::api::RobotIdResponse> Robot::GetId(const std::string& id_service_name) {
    Result<::bosdyn::client::RobotIdClient*> robot_id_client_result =
        EnsureServiceClient<::bosdyn::client::RobotIdClient>(id_service_name);
    if (!robot_id_client_result.status) {
        return {robot_id_client_result.status, {}};
    }

    return robot_id_client_result.response->Get();
}

std::string Robot::GetTokenId(const std::string& username) {
    std::string token_id = m_serial_number + "." + username;
    return token_id;
}

Result<std::vector<::bosdyn::api::ServiceEntry>> Robot::ListServices() {
    Result<std::shared_ptr<grpc::ChannelInterface>> dir_channel_result =
        EnsureChannel(DirectoryClient::GetDefaultServiceName(), DirectoryClient::GetServiceType());
    if (!dir_channel_result.status) {
        return {dir_channel_result.status, {}};
    }
    Result<DirectoryClient*> dir_client_result = EnsureServiceClient<DirectoryClient>(
        DirectoryClient::GetDefaultServiceName(), dir_channel_result.response);
    if (!dir_client_result.status) {
        return {dir_client_result.status, {}};
    }

    // If communication is bypassing the proxy, we call ListServicePairs RPC. Otherwise, we call
    // ListServiceEntries RPC.
        return UpdateInformationFromListEntries(dir_client_result.response);
}


Result<std::vector<::bosdyn::api::ServiceEntry>> Robot::UpdateInformationFromListEntries(
    ::bosdyn::client::DirectoryClient* dir_client) {
    std::vector<::bosdyn::api::ServiceEntry> service_list;
    DirectoryListResultType result = dir_client->ListServiceEntries();
    if (!result.status) {
        return {result.status, {}};
    }

    m_authorities_by_name.clear();
    for (const auto& entry : result.response.service_entries()) {
        service_list.push_back(entry);
        m_authorities_by_name[entry.name()] = entry.authority();
    }

    return {::bosdyn::common::Status(SDKErrorCode::Success), std::move(service_list)};
}


::bosdyn::common::Status Robot::StartTimeSync() {
    auto set_status = SetTimeSyncMembers();
    if (!set_status) {
        return set_status;
    }
    m_time_sync_thread->Start();
    return ::bosdyn::common::Status(SDKErrorCode::Success);
}

void Robot::StopTimeSync() {
    if (m_time_sync_thread != nullptr) {
        m_time_sync_thread->Stop();
    }
}

Result<std::shared_ptr<TimeSyncThread>> Robot::GetTimeSyncThread() {
    auto start_time_sync_status = StartTimeSync();
    if (!start_time_sync_status) {
        return {start_time_sync_status, nullptr};
    }
    return {::bosdyn::common::Status(SDKErrorCode::Success), m_time_sync_thread};
}

Result<TimeSyncEndpoint*> Robot::StartTimeSyncAndGetEndpoint() {
    // Establish time synchronization with the robot object.
    auto time_sync_thread_res = this->GetTimeSyncThread();
    if (!time_sync_thread_res.status) {
        return {time_sync_thread_res.status, nullptr};
    }
    bool synced = time_sync_thread_res.response->WaitForSync(std::chrono::seconds(30));
    if (!synced) {
        return {::bosdyn::common::Status(SDKErrorCode::GenericSDKError,
                                         "ERROR, could not establish timesync in 30 seconds."),
                nullptr};
    }
    return {::bosdyn::common::Status(SDKErrorCode::Success),
            time_sync_thread_res.response->GetEndpoint()};
}

bool Robot::ShouldUseInsecureChannel(const std::string& service_name,
                                     const std::string& host) const {
    /*
      The check whether to use insecure channels is as follows:
       - If service is in bootstrap list; this is the case when the code is making the first
         call to Directory client to sync endpoints and authorities:
           - return true
       - If host == Directory service host; this is the case when service is running
            on the same host as Directory, which is assumed to be running on P1:
           - return true
       - If none of the checks above return True, that means the service is not running on P1,
            therefore, we need to go through the proxy.
           - return false
    */
    if (m_bootstrap_endpoints_by_name.find(service_name) != m_bootstrap_endpoints_by_name.end()) {
        return true;
    }

    if (!m_directory_host.empty() && host == m_directory_host) {
        return true;
    }

    return false;
}

::bosdyn::common::Status Robot::SetTimeSyncMembers() {
    auto client_result = EnsureServiceClient<TimeSyncClient>();
    if (!client_result.status) {
        return client_result.status;
    }
    m_time_sync_client = client_result.response;
    m_time_sync_thread = std::make_unique<TimeSyncThread>(m_time_sync_client);
    return ::bosdyn::common::Status(SDKErrorCode::Success);
}

::bosdyn::common::Status Robot::PowerOnMotors(::bosdyn::common::Duration timeout,
                                              double update_frequency) {
    Result<PowerClient*> power_client_result =
        EnsureServiceClient<PowerClient>(PowerClient::GetDefaultServiceName());
    if (!power_client_result) {
        return power_client_result.status;
    }

    return power_client_helper::PowerOnMotors(power_client_result.response, timeout,
                                              update_frequency);
}

::bosdyn::common::Status Robot::PowerOffMotors(bool cut_immediately,
                                               ::bosdyn::common::Duration timeout,
                                               double update_frequency) {
    Result<PowerClient*> power_client_result =
        EnsureServiceClient<PowerClient>(PowerClient::GetDefaultServiceName());
    if (!power_client_result) {
        return power_client_result.status;
    }

    if (cut_immediately) {
        return power_client_helper::PowerOffMotors(power_client_result.response, timeout,
                                                   update_frequency);
    }

    Result<RobotCommandClient*> robot_command_client_result =
        EnsureServiceClient<RobotCommandClient>(RobotCommandClient::GetDefaultServiceName());
    if (!robot_command_client_result) {
        return robot_command_client_result.status;
    }

    Result<RobotStateClient*> robot_state_client_result =
        EnsureServiceClient<RobotStateClient>(RobotStateClient::GetDefaultServiceName());
    if (!robot_state_client_result) {
        return robot_state_client_result.status;
    }

    return power_client_helper::SafePowerOffMotors(robot_command_client_result.response,
                                                   robot_state_client_result.response, timeout,
                                                   update_frequency);
}

Result<bool> Robot::IsPoweredOn() {
    Result<RobotStateClient*> robot_state_client_result =
        EnsureServiceClient<RobotStateClient>(RobotStateClient::GetDefaultServiceName());
    if (!robot_state_client_result) {
        return {robot_state_client_result.status, false};
    }

    return power_client_helper::IsPoweredOn(robot_state_client_result.response);
}

Result<std::shared_ptr<const ::bosdyn::api::FrameTreeSnapshot>> Robot::GetFrameTreeSnapshot() {
    Result<RobotStateClient*> robot_state_client_result =
        EnsureServiceClient<RobotStateClient>(RobotStateClient::GetDefaultServiceName());
    if (!robot_state_client_result) {
        return {robot_state_client_result.status, nullptr};
    }

    RobotStateResultType result = robot_state_client_result.response->GetRobotState();
    if (!result) {
        return {result.status, nullptr};
    }
    std::shared_ptr<const ::bosdyn::api::FrameTreeSnapshot> snapshot_ptr =
        std::make_shared<const ::bosdyn::api::FrameTreeSnapshot>(
            result.response.robot_state().kinematic_state().transforms_snapshot());
    return {::bosdyn::common::Status(SDKErrorCode::Success), snapshot_ptr};
}

void Robot::UpdateUserToken(const std::string& user_token, const std::string& username) {
    {
        std::lock_guard<std::mutex> lock(m_token_mutex);
        m_user_token = user_token;
    }
    UpdateTokenCache(username);
}

const std::string& Robot::GetUserToken() {
    std::lock_guard<std::mutex> lock(m_token_mutex);
    return m_user_token;
}

void Robot::SetRPCParameters(const RPCParameters& parameters) {
    m_RPC_parameters = parameters;

    // Update any existing service clients;
    for (auto& client_it : m_service_client_map) {
        client_it.second.service_client->SetRPCParameters(parameters);
    }
}

Result<bool> Robot::IsEstopped() {
    auto estop_client_result = EnsureServiceClient<EstopClient>();
    if (!estop_client_result) {
        return {estop_client_result.status, false};
    }

    auto estop_status = estop_client_result.response->GetEstopStatus();
    if (!estop_status) {
        return {estop_status.status, false};
    }

    return {::bosdyn::common::Status(SDKErrorCode::Success),
            estop_status.response.status().stop_level() != ::bosdyn::api::ESTOP_LEVEL_NONE};
}

}  // namespace client

}  // namespace bosdyn
