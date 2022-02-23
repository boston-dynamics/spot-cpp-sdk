/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/docking/docking_client.h"
#include "bosdyn/common/success_condition.h"
#include "bosdyn/client/lease/lease_processors.h"
#include "bosdyn/client/lease/lease_resources.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* DockingClient::s_default_service_name = "docking";

const char* DockingClient::s_service_type = "bosdyn.api.docking.DockingService";

std::shared_future<GetDockingConfigResultType> DockingClient::GetDockingConfigAsync(
    const RPCParameters& parameters) {
    std::promise<GetDockingConfigResultType> response;
    std::shared_future<GetDockingConfigResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::docking::GetDockingConfigRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::docking::GetDockingConfigRequest,
                          ::bosdyn::api::docking::GetDockingConfigResponse,
                          ::bosdyn::api::docking::GetDockingConfigResponse>(
            request,
            std::bind(&::bosdyn::api::docking::DockingService::Stub::AsyncGetDockingConfig, m_stub.get(), _1,
                      _2, _3),
            std::bind(&DockingClient::OnGetDockingConfigComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

GetDockingConfigResultType DockingClient::GetDockingConfig(
    const RPCParameters& parameters) {
    return GetDockingConfigAsync(parameters).get();
}

void DockingClient::OnGetDockingConfigComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::docking::GetDockingConfigRequest& request,
    ::bosdyn::api::docking::GetDockingConfigResponse&& response, const grpc::Status& status,
    std::promise<GetDockingConfigResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::docking::GetDockingConfigResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<GetDockingStateResultType> DockingClient::GetDockingStateAsync(
    const RPCParameters& parameters) {
    std::promise<GetDockingStateResultType> response;
    std::shared_future<GetDockingStateResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::docking::GetDockingStateRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::docking::GetDockingStateRequest,
                          ::bosdyn::api::docking::GetDockingStateResponse,
                          ::bosdyn::api::docking::GetDockingStateResponse>(
            request,
            std::bind(&::bosdyn::api::docking::DockingService::Stub::AsyncGetDockingState, m_stub.get(), _1,
                      _2, _3),
            std::bind(&DockingClient::OnGetDockingStateComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

GetDockingStateResultType DockingClient::GetDockingState(
    const RPCParameters& parameters) {
    return GetDockingStateAsync(parameters).get();
}

void DockingClient::OnGetDockingStateComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::docking::GetDockingStateRequest& request,
    ::bosdyn::api::docking::GetDockingStateResponse&& response, const grpc::Status& status,
    std::promise<GetDockingStateResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::docking::GetDockingStateResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<DockingCommandResultType> DockingClient::DockingCommandAsync(
    ::bosdyn::api::docking::DockingCommandRequest& request,
    const RPCParameters& parameters) {
    std::promise<DockingCommandResultType> response;
    std::shared_future<DockingCommandResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");
    // Run a lease processor function to attempt to automatically apply a lease to the request if
    // a lease is not already set.
    auto lease_status = ::bosdyn::client::ProcessRequestWithLease(
        &request, m_lease_wallet.get(), ::bosdyn::client::kBodyResource);
    if (!lease_status) {
        // Failed to set a lease with the lease wallet. Return early since the request will fail without a lease.
        response.set_value({lease_status, {}});
        return future;
    }

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::docking::DockingCommandRequest, ::bosdyn::api::docking::DockingCommandResponse,
                          ::bosdyn::api::docking::DockingCommandResponse>(
            request,
            std::bind(&::bosdyn::api::docking::DockingService::Stub::AsyncDockingCommand, m_stub.get(), _1,
                      _2, _3),
            std::bind(&DockingClient::OnDockingCommandComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

DockingCommandResultType DockingClient::DockingCommand(
    ::bosdyn::api::docking::DockingCommandRequest& request,
    const RPCParameters& parameters) {
    return DockingCommandAsync(request, parameters).get();
}

void DockingClient::OnDockingCommandComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::docking::DockingCommandRequest& request,
    ::bosdyn::api::docking::DockingCommandResponse&& response, const grpc::Status& status,
    std::promise<DockingCommandResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseWithLeaseAndGetFinalStatus<::bosdyn::api::docking::DockingCommandResponse>(
            status, response, response.status(), m_lease_wallet.get());

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<DockingCommandFeedbackResultType> DockingClient::DockingCommandFeedbackAsync(
    unsigned int id, const RPCParameters& parameters) {
    ::bosdyn::api::docking::DockingCommandFeedbackRequest request;
    request.set_docking_command_id(id);
    return DockingCommandFeedbackAsync(request, parameters);
}

std::shared_future<DockingCommandFeedbackResultType> DockingClient::DockingCommandFeedbackAsync(
    ::bosdyn::api::docking::DockingCommandFeedbackRequest& request,
    const RPCParameters& parameters) {
    std::promise<DockingCommandFeedbackResultType> response;
    std::shared_future<DockingCommandFeedbackResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::docking::DockingCommandFeedbackRequest,
                          ::bosdyn::api::docking::DockingCommandFeedbackResponse,
                          ::bosdyn::api::docking::DockingCommandFeedbackResponse>(
            request,
            std::bind(&::bosdyn::api::docking::DockingService::Stub::AsyncDockingCommandFeedback,
                      m_stub.get(), _1, _2, _3),
            std::bind(&DockingClient::OnDockingCommandFeedbackComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

DockingCommandFeedbackResultType DockingClient::DockingCommandFeedback(
    ::bosdyn::api::docking::DockingCommandFeedbackRequest& request,
    const RPCParameters& parameters) {
    return DockingCommandFeedbackAsync(request, parameters).get();
}

DockingCommandFeedbackResultType DockingClient::DockingCommandFeedback(
    unsigned int id, const RPCParameters& parameters) {
    return DockingCommandFeedbackAsync(id, parameters).get();
}

void DockingClient::OnDockingCommandFeedbackComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::docking::DockingCommandFeedbackRequest& request,
    ::bosdyn::api::docking::DockingCommandFeedbackResponse&& response, const grpc::Status& status,
    std::promise<DockingCommandFeedbackResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseAndGetFinalStatus<::bosdyn::api::docking::DockingCommandFeedbackResponse>(
            status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

Result<::bosdyn::api::docking::DockingCommandRequest> DockingClient::DockingCommandBuilder(
    unsigned int dock_id, ::bosdyn::common::TimePoint local_end_time,
    TimeSyncEndpoint* time_sync_endpoint) {
    Result<::bosdyn::api::docking::DockingCommandRequest> result;
    auto& req = result.response;

    // Lease is handled automatically via processor

    // ID
    req.set_docking_station_id(dock_id);

    // Timesync
    TimeSyncEndpoint* endpoint = time_sync_endpoint ? time_sync_endpoint : m_time_sync_endpoint;
    if (endpoint) {
        req.set_clock_identifier(*endpoint->GetClockIdentifier().response);
        req.mutable_end_time()->CopyFrom(endpoint->RobotTimestampFromLocal(local_end_time));
    } else {
        result.status = ::bosdyn::common::Status(SDKErrorCode::GenericSDKError,
                               "Timesync endpoint is unset for the Docking Client.");
        return result;
    }

    result.status = ::bosdyn::common::Status(SDKErrorCode::Success);
    return result;
}

// Start of ServiceClient overrides.
ServiceClient::QualityOfService DockingClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void DockingClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::docking::DockingService::Stub(channel));
}

void DockingClient::UpdateServiceFrom(
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
