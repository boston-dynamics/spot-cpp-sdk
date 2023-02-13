/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/geometry.pb.h>
#include <string>
#include "bosdyn/client/directory/directory_client.h"
#include "bosdyn/client/error_codes/client_creation_error_code.h"
#include "bosdyn/client/lease/lease_wallet.h"
#include "bosdyn/client/processors/request_processor.h"
#include "bosdyn/client/processors/response_processor.h"
#include "bosdyn/client/robot_id/robot_id_client.h"
#include "bosdyn/client/service_client/message_pump.h"
#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/client/time_sync/time_sync_helpers.h"
#include "token_cache.h"
#include "token_manager.h"

namespace bosdyn {

namespace client {

class TokenManager;  // Forward class declaration to resolve the circular dependency

// The endpoint representation of an internal service
struct Endpoint {
    std::string host_ip;
    int port;
    std::string service_type;

    // Convert struct to a string representation to be used as the key in endpoints map.
    std::string GetEndpointString() const { return host_ip + "_" + std::to_string(port); }
};

// Robot represents a single user on a single robot. It manages user credentials and communications
// to the Robot, and is used to create clients for specific services on the robot.

class Robot {
 public:
    /**
     * Robot constructor.
     *
     * @param bypass_proxy(boolean): Boolean whether the proxy should be bypassed in case the
     *            connections are all internal to the robot computer, or the robot firewall is down.
     *            Defaulted to false.
     * @param timeout(::bosdyn::common::Duration): Duration for RPC timeouts in all
     *            the clients created from this robot instance. Defaulted to  field default value,
     *            which represents no timeout set at this level and the ServiceClient default
     *            timeout is used.
     */
    explicit Robot(const std::string& client_name, bool bypass_proxy = false,
                   ::bosdyn::common::Duration timeout = kRPCTimeoutNotSpecified);

    ~Robot();

    // Authenticate with user credentials.
    ::bosdyn::common::Status Authenticate(const std::string& username, const std::string& password);

    // Authenticate with user token.
    ::bosdyn::common::Status AuthenticateWithToken(const std::string& token);

    // Ensures that a ServiceClient is created (templatized version of the method above).
    // service_name has to match one of the ServiceClientFactories registered with the
    // ClientSdk/Robot channel is optional GRPC channel to use in the ServiceClient, new one is
    // created if this
    //   parameter is not passed to the method.
    // message_pump is optional MessagePump instance to use in the ServiceClient, the default
    //   MessagePump for the robot will be used if this parameter is not passed to the method.
    template <class T>
    Result<T*> EnsureServiceClient(const std::string& service_name,
                                   std::shared_ptr<grpc::ChannelInterface> channel = nullptr,
                                   std::shared_ptr<MessagePump> message_pump = nullptr) {
        // Use an existing client if one exists.
        auto it_client = m_service_client_map.find(service_name);
        if (it_client != m_service_client_map.end()) {
            if (T::GetServiceType() != it_client->second.service_type) {
                return {::bosdyn::common::Status(
                            ClientCreationErrorCode::IncorrectServiceType,
                            "Cached service client for " + service_name + " has service type " +
                                it_client->second.service_type +
                                ". The expected type is: " + T::GetServiceType()),
                        nullptr};
            }
            return {::bosdyn::common::Status(ClientCreationErrorCode::Success),
                    static_cast<T*>(it_client->second.service_client.get())};
        }

        // Since no client currently exists for this service name, create a new one using the
        // default constructor of the templated service client argument.

        // The mutex is necessary so multiple threads can create clients at the same time, which
        // could potentially trigger the call of this method and generate two directory
        // clients/channels at the same time, adding the same entry in the ServiceClientMap at the
        // same time.
        std::lock_guard<std::recursive_mutex> lock(m_client_create_mutex);
        std::unique_ptr<T> client = std::make_unique<T>();
        auto status =
            SetupClient(client.get(), service_name, T::GetServiceType(), channel, message_pump);
        if (status) {
            // Ultimately, add the new service client into the service client map such that it can
            // be reused if there is another request for the same service name.
            m_service_client_map[service_name] = {T::GetServiceType(), std::move(client)};
            return {status,
                    static_cast<T*>(m_service_client_map[service_name].service_client.get())};
        } else {
            return {status, nullptr};
        }
    }

    // As above, except the service_name is inferred from the type.
    template <class T>
    Result<T*> EnsureServiceClient(std::shared_ptr<grpc::ChannelInterface> channel = nullptr,
                                   std::shared_ptr<MessagePump> message_pump = nullptr) {
        return EnsureServiceClient<T>(T::GetDefaultServiceName(), channel, message_pump);
    }

    // NOTE: Most of the following methods are called by ClientSdk and not typically accessed
    // directly by SDK users.

    // Set the network address that the robot can be reached at.
    void SetNetworkAddress(const std::string& network_address);

    // Set the Request and Response processor chains which intercept all RPCs made by all clients.
    // The processors referenced in the chains must be kept alive for the entire lifetime of the
    // robot.
    void SetGlobalProcessorChains(const RequestProcessorChain& request_processor_chain,
                                  const ResponseProcessorChain& response_processor_chain);

    // Add a custom request processor for any ServiceClient's generated by this robot. The processor
    // must be kept alive for the entire lifetime of the robot.
    void AddCustomRequestProcessor(const std::shared_ptr<RequestProcessor>& processor);

    // Add a custom response processor for any ServiceClient's generated by this robot. The
    // processor must be kept alive for the entire lifetime of the robot.
    void AddCustomResponseProcessor(const std::shared_ptr<ResponseProcessor>& processor);

    // Set the lease wallet to be used for future clients.
    void SetWallet(std::shared_ptr<LeaseWallet> wallet) { m_lease_wallet = wallet; }

    // Access the robot's wallet, which should be shared between its clients.
    std::shared_ptr<LeaseWallet> GetWallet() { return m_lease_wallet; }

    // Update user token in the robot.
    void UpdateUserToken(const std::string& user_token, const std::string& username = "");

    const std::string& GetUserToken();

    // Set certificate in the robot.
    void SetRobotCert(const std::string& cert) { m_cert = cert; }

    // Sets up a token cache to persist the user token.
    // If the user provides a cache, it will be saved in the robot object for convenience.
    ::bosdyn::common::Status SetupTokenCache(std::unique_ptr<TokenCache> token_cache,
                                             const std::string& unique_id = "");

    // List services available on the robot.
    Result<std::vector<::bosdyn::api::ServiceEntry>> ListServices();


    // Start the time sync thread if it is not already running.
    ::bosdyn::common::Status StartTimeSync();

    // Stop the time sync thread if it is running.
    void StopTimeSync();

    // Return a pointer to the TimesyncEndpoint from which the timesync thread has started using and
    // establishing sync with the robot's clock.
    Result<TimeSyncEndpoint*> StartTimeSyncAndGetEndpoint();

    // Return pointer to the TimeSyncThread. Start if not yet started.
    Result<std::shared_ptr<TimeSyncThread>> GetTimeSyncThread();

    // Set the default MessagePump instance to be used by the ServiceClient instances used by the
    // robot.
    void SetDefaultMessagePump(std::shared_ptr<MessagePump> message_pump) {
        m_default_message_pump = message_pump;
    }


    /**
     * Power on robot motors. This function blocks until the command returns success, or the timeout
     * is reached.
     *
     * @param timeout (::bosdyn::common::Duration): Max time this function will block for.
     * @param update_frequency (double): The frequency with which the robot should check if the
     *                                  command has succeeded.
     * @return ::bosdyn::common::Status object with error code, or Success code.
     */
    ::bosdyn::common::Status PowerOnMotors(
        ::bosdyn::common::Duration timeout = std::chrono::seconds(30),
        double update_frequency = 1.0);

    /**
     * Power off robot motors.
     *
     * @param cut_immediately (bool): Power motors off immediately (true) or safely (false).
     * @param timeout (::bosdyn::common::Duration): Max time this function will block for.
     * @param update_frequency (double): The frequency with which the robot should check if the
     *                                  command has succeeded.
     * @return ::bosdyn::common::Status object with error code, or Success code.
     */
    ::bosdyn::common::Status PowerOffMotors(
        bool cut_immediately = false, ::bosdyn::common::Duration timeout = std::chrono::seconds(30),
        double update_frequency = 1.0);

    /**
     * Check if the robot is powered on. This function blocks until the robot state is received.
     *
     * @param robot_state_client (::bosdyn::api::RobotStateClient): client for calling robot_state
     *                                                            service.
     * @return Result with ::bosdyn::common::Status object with error code if errors are found, or a
     * Success Status object and a bool response whether the robot is powered on or not, if no
     * errors are found.
     */
    Result<bool> IsPoweredOn();

    Result<std::shared_ptr<const ::bosdyn::api::FrameTreeSnapshot>> GetFrameTreeSnapshot();

    /**
     * Set RPC parameters in existing service client and new ones created from this point on. This
     * operation will not affect RPCs currently in flight.
     *
     * @param parameters (::bosdyn::client::RPCParameters): New RPC parameters to use.
     */
    void SetRPCParameters(const RPCParameters& parameters);


    Result<::bosdyn::api::RobotIdResponse> GetId(
        const std::string& id_service_name =
            ::bosdyn::client::RobotIdClient::GetDefaultServiceName());

    /**
     * Check if the robot is estopped, usually indicating if an external application has not
     registered and held an estop endpoint.
     *
     * @return: True if robot is estopped, False otherwise.
     */
    Result<bool> IsEstopped();

    /**
     * @brief Update the port used for creating secure channels, instead of using the default 443
     * port. Calling this method does not change existing channels.
     *
     * @param secure_channel_port New port to use for creating secure channels.
     */
    void UpdateSecureChannelPort(int secure_channel_port) {
        m_secure_channel_port = secure_channel_port; }

 protected:
    // Updates the cache with the existing user token.
    // This method also instantiates a token manager to refresh the
    // user token.  Furthermore, it should only be called after the
    // token has been retrieved.
    void UpdateTokenCache(const std::string& username = "");

    std::string GetTokenId(const std::string& username);

    // Create/find GRPC channel.
    Result<std::shared_ptr<grpc::ChannelInterface>> EnsureChannel(const std::string& service_name,
                                                                  const std::string& service_type);

    // Create/find secure GRPC channel for communication with the robot for a specific authority.
    Result<std::shared_ptr<grpc::ChannelInterface>> EnsureSecureChannel(
        const std::string& authority);

    // Create/find insecure GRPC channel for communication with a specific service on the robot.
    Result<std::shared_ptr<grpc::ChannelInterface>> EnsureInsecureChannel(const Endpoint& endpoint);

    // Get list of services and update internal information from the ListServiceEntries Directory
    // service RPC.
    Result<std::vector<::bosdyn::api::ServiceEntry>> UpdateInformationFromListEntries(
        ::bosdyn::client::DirectoryClient* dir_client);

    // Get list of services and update internal information from the ListServicePairs Directory
    // service RPC.
    Result<std::vector<::bosdyn::api::ServiceEntry>> UpdateInformationFromListPairs(
        ::bosdyn::client::DirectoryClient* dir_client);

    // Check if host of the service endpoint is a local address so we can
    // connect through insecure channels. This method is only called when bypass_proxy is set to
    // true.
    bool ShouldUseInsecureChannel(const std::string& service_name, const std::string& host) const;

    // Create TimeSyncClient and set time sync members.
    ::bosdyn::common::Status SetTimeSyncMembers();

    // Struct used to store the cached service clients for reuse.
    struct CachedServiceClient {
        // Service type as returned by the client's GetServiceType method.
        std::string service_type;
        // Cached service client.
        std::unique_ptr<ServiceClient> service_client;
    };

    // This maps holds shared_ptr for ServiceClients so they can be shared with applications
    // while blocking duplicate instances.
    typedef std::map<std::string, CachedServiceClient> ServiceClientMap;
    ServiceClientMap m_service_client_map;

    // Helper function to setup the ServiceClient after it is created.
    ::bosdyn::common::Status SetupClient(ServiceClient* service_client,
                                         const std::string& service_name,
                                         const std::string& service_type,
                                         std::shared_ptr<grpc::ChannelInterface> channel = nullptr,
                                         std::shared_ptr<MessagePump> message_pump = nullptr);

    std::string m_network_address;
    // Host string as it appears in Directory:ListPairsResponse reported for Directory service.
    std::string m_directory_host;

    RequestProcessorChain m_request_processor_chain;
    ResponseProcessorChain m_response_processor_chain;

    std::string m_cert;

    // This holds the default MessagePump instance to be used by all ServiceClient instances.
    std::shared_ptr<MessagePump> m_default_message_pump = nullptr;

    std::shared_ptr<LeaseWallet> m_lease_wallet;
    std::string m_user_token;
    std::unique_ptr<TokenManager> m_token_manager;
    std::string m_current_user;
    std::unique_ptr<TokenCache> m_token_cache;
    std::string m_serial_number;

    // Map with bootstrapped authorities for the services we need to communicate with before
    // getting the full list from the Directory service. Key in the map represents the service
    // name and value represents the authority.
    const std::map<std::string, std::string> m_bootstrap_authorities_by_name;

    // Map to store authorities for the services as received from the Directory service. Key in the
    // map represents the service name and value represents the authority.
    std::map<std::string, std::string> m_authorities_by_name;

    // Map that stores the channels created so far. This map behaves differently for internal and
    // external instances. The map keys represent authorities in external instances and "host:port"
    // string in internal instances.
    std::map<std::string, std::shared_ptr<grpc::ChannelInterface>> m_channels;

    // Boolean to store whether Robot instance should bypass the proxy when creating the
    // ServiceClients.
    bool m_bypass_proxy = false;

    // For internal Robot instances, the following map stores the bootstrap endpoints.
    const std::map<std::string, Endpoint> m_bootstrap_endpoints_by_name;

    // For internal Robot instances, the following map stores the endpoint for each service.
    std::map<std::string, Endpoint> m_endpoints_by_name;

    // TimeSyncClient and TimeSyncThread for maintaining non-block time sync with the robot.
    TimeSyncClient* m_time_sync_client = nullptr;
    std::shared_ptr<TimeSyncThread> m_time_sync_thread;

    // Mutex for managing the update of m_user_token.
    std::mutex m_token_mutex;

    // Mutex for blocking the creation of two directory clients in parallel.
    std::recursive_mutex m_client_create_mutex;

    // Parameters for RPC calls.
    RPCParameters m_RPC_parameters;

    // Secure port to use when creating secure channels; defaults to 443.
    int m_secure_channel_port;
};

}  // namespace client

}  // namespace bosdyn
