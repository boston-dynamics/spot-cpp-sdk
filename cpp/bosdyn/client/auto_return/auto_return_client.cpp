/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/auto_return/auto_return_client.h"
#include "bosdyn/common/success_condition.h"
#include "bosdyn/client/lease/lease_processors.h"
#include "bosdyn/client/lease/lease_resources.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* AutoReturnClient::s_default_service_name = "auto-return";

const char* AutoReturnClient::s_service_type = "bosdyn.api.auto_return.AutoReturnService";

std::shared_future<ConfigureResultType> AutoReturnClient::ConfigureAsync(
    ::bosdyn::api::auto_return::ConfigureRequest& request,
    const RPCParameters& parameters) {
    std::promise<ConfigureResultType> response;
    std::shared_future<ConfigureResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");
    // Run a lease processor function to attempt to automatically apply a lease to the request if
    // a lease is not already set.
    auto lease_status = ::bosdyn::client::ProcessRequestWithMultipleLeases(
        &request, m_lease_wallet.get(), {::bosdyn::client::kBodyResource});
    if (!lease_status) {
        // Failed to set a lease with the lease wallet. Return early since the request will fail
        // without a lease.
        response.set_value({lease_status, {}});
        return future;
    }

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::auto_return::ConfigureRequest, ::bosdyn::api::auto_return::ConfigureResponse,
                          ::bosdyn::api::auto_return::ConfigureResponse>(
            request,
            std::bind(&::bosdyn::api::auto_return::AutoReturnService::Stub::AsyncConfigure, m_stub.get(), _1,
                      _2, _3),
            std::bind(&AutoReturnClient::OnConfigureComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

ConfigureResultType AutoReturnClient::Configure(
    ::bosdyn::api::auto_return::ConfigureRequest& request,
    const RPCParameters& parameters) {
    return ConfigureAsync(request, parameters).get();
}

void AutoReturnClient::OnConfigureComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::auto_return::ConfigureRequest& request,
    ::bosdyn::api::auto_return::ConfigureResponse&& response, const grpc::Status& status,
    std::promise<ConfigureResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::auto_return::ConfigureResponse>(
        status, response, response.status());
    promise.set_value({ret_status, std::move(response)});
}


std::shared_future<GetConfigurationResultType> AutoReturnClient::GetConfigurationAsync(
    ::bosdyn::api::auto_return::GetConfigurationRequest& request,
    const RPCParameters& parameters) {
    std::promise<GetConfigurationResultType> response;
    std::shared_future<GetConfigurationResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::auto_return::GetConfigurationRequest,
                          ::bosdyn::api::auto_return::GetConfigurationResponse,
                          ::bosdyn::api::auto_return::GetConfigurationResponse>(
            request,
            std::bind(&::bosdyn::api::auto_return::AutoReturnService::Stub::AsyncGetConfiguration,
                      m_stub.get(), _1, _2, _3),
            std::bind(&AutoReturnClient::OnGetConfigurationComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

GetConfigurationResultType AutoReturnClient::GetConfiguration(
    ::bosdyn::api::auto_return::GetConfigurationRequest& request,
    const RPCParameters& parameters) {
    return GetConfigurationAsync(request, parameters).get();
}

void AutoReturnClient::OnGetConfigurationComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::auto_return::GetConfigurationRequest& request,
    ::bosdyn::api::auto_return::GetConfigurationResponse&& response, const grpc::Status& status,
    std::promise<GetConfigurationResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseAndGetFinalStatus<::bosdyn::api::auto_return::GetConfigurationResponse>(
            status, response, SDKErrorCode::Success);
    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<StartResultType> AutoReturnClient::StartAsync(
    ::bosdyn::api::auto_return::StartRequest& request,
    const RPCParameters& parameters) {
    std::promise<StartResultType> response;
    std::shared_future<StartResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::auto_return::StartRequest, ::bosdyn::api::auto_return::StartResponse,
                          ::bosdyn::api::auto_return::StartResponse>(
            request,
            std::bind(&::bosdyn::api::auto_return::AutoReturnService::Stub::AsyncStart, m_stub.get(), _1,
                      _2, _3),
            std::bind(&AutoReturnClient::OnStartComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

StartResultType AutoReturnClient::Start(
    ::bosdyn::api::auto_return::StartRequest& request,
    const RPCParameters& parameters) {
    return StartAsync(request, parameters).get();
}

void AutoReturnClient::OnStartComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::auto_return::StartRequest& request,
    ::bosdyn::api::auto_return::StartResponse&& response, const grpc::Status& status,
    std::promise<StartResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::auto_return::StartResponse>(
        status, response, SDKErrorCode::Success);
    promise.set_value({ret_status, std::move(response)});
}


::bosdyn::common::Status AutoReturnClient::Init(const Lease& new_lease,
                              const ::bosdyn::api::auto_return::Params& params,
                              const std::string& client_name,
                              std::shared_ptr<LeaseWallet> lease_wallet) {
    // Create a lease for auto_return of the form [A, B, ... , (N+1)].
    Lease auto_return_lease = new_lease.Increment();

    // Send the request to configure.
    ::bosdyn::api::auto_return::ConfigureRequest req;
    req.add_leases();
    req.mutable_leases(0)->CopyFrom(auto_return_lease.GetProto());
    req.mutable_params()->CopyFrom(params);
    ::bosdyn::client::ConfigureResultType configure_auto_return_result = Configure(req);
    if (!configure_auto_return_result) {
        return configure_auto_return_result.status.Chain("Failed to configure auto return");
    }

    std::string cmd_lease_str = "UNSET";
    // Only update the lease wallet if we successfully configured AutoReturn.
    // Set the lease for commands to [A, B, ... , N, 0].
    // (The final '0' is added automatically by AddLease, when the sublease is made).
    if (lease_wallet) {
        lease_wallet->AddLease(new_lease);
        cmd_lease_str =
            lease_wallet->GetLease(new_lease.GetResource()).response.GetProto().DebugString();
    }
    return ::bosdyn::common::Status(SDKErrorCode::Success,
                  "Successfully initialized auto return. Command lease: " +
                      cmd_lease_str +
                      " Auto return lease: " + auto_return_lease.GetProto().DebugString());
}

// Start of ServiceClient overrides.
ServiceClient::QualityOfService AutoReturnClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void AutoReturnClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::auto_return::AutoReturnService::Stub(channel));
}

void AutoReturnClient::UpdateServiceFrom(
    RequestProcessorChain& request_processor_chain,
    ResponseProcessorChain& response_processor_chain,
    const std::shared_ptr<LeaseWallet>& lease_wallet) {
    // Set the lease wallet.
    m_lease_wallet = lease_wallet;

    // Set the request and response processors.
    m_request_processor_chain = request_processor_chain;
    m_response_processor_chain = response_processor_chain;
}
// End of ServiceClient overrides.

}  // namespace client

}  // namespace bosdyn
