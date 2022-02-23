/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/graph_nav/recording_client.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"
#include "bosdyn/client/lease/lease_processors.h"
#include "bosdyn/client/lease/lease_resources.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* GraphNavRecordingClient::s_default_service_name = "recording-service";

const char* GraphNavRecordingClient::s_service_type =
    "bosdyn.api.graph_nav.GraphNavRecordingService";

std::shared_future<StartRecordingResultType> GraphNavRecordingClient::StartRecordingAsync(
    ::bosdyn::api::graph_nav::StartRecordingRequest& request, const RPCParameters& parameters) {
    std::promise<StartRecordingResultType> response;
    std::shared_future<StartRecordingResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");
    // Run a lease processor function to attempt to automatically apply a lease to the request if
    // a lease is not already set.
    auto lease_status = ProcessRequestWithLease(&request, m_lease_wallet.get(),
                                                ::bosdyn::client::kBodyResource);
    if (!lease_status) {
        // Failed to set a lease with the lease wallet. Return early since the request will fail without a lease.
        response.set_value({lease_status, {}});
        return future;
    }

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::graph_nav::StartRecordingRequest,
                          ::bosdyn::api::graph_nav::StartRecordingResponse,
                          ::bosdyn::api::graph_nav::StartRecordingResponse>(
            request,
            std::bind(&::bosdyn::api::graph_nav::GraphNavRecordingService::Stub::AsyncStartRecording,
                      m_stub.get(), _1, _2, _3),
            std::bind(&GraphNavRecordingClient::OnStartRecordingComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

StartRecordingResultType GraphNavRecordingClient::StartRecording(
    ::bosdyn::api::graph_nav::StartRecordingRequest& request, const RPCParameters& parameters) {
    return StartRecordingAsync(request, parameters).get();
}

void GraphNavRecordingClient::OnStartRecordingComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::StartRecordingRequest& request,
    ::bosdyn::api::graph_nav::StartRecordingResponse&& response, const grpc::Status& status,
    std::promise<StartRecordingResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseWithLeaseAndGetFinalStatus<::bosdyn::api::graph_nav::StartRecordingResponse>(
            status, response, response.status(), m_lease_wallet.get());

    promise.set_value(
        {ret_status, std::move(response)});
}

std::shared_future<StopRecordingResultType> GraphNavRecordingClient::StopRecordingAsync(
    ::bosdyn::api::graph_nav::StopRecordingRequest& request, const RPCParameters& parameters) {
    std::promise<StopRecordingResultType> response;
    std::shared_future<StopRecordingResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");
    // Run a lease processor function to attempt to automatically apply a lease to the request if
    // a lease is not already set.
    auto lease_status = ProcessRequestWithLease(&request, m_lease_wallet.get(),
                                                ::bosdyn::client::kBodyResource);
    if (!lease_status) {
        // Failed to set a lease with the lease wallet. Return early since the request will fail without a lease.
        response.set_value({lease_status, {}});
        return future;
    }

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::graph_nav::StopRecordingRequest,
                          ::bosdyn::api::graph_nav::StopRecordingResponse,
                          ::bosdyn::api::graph_nav::StopRecordingResponse>(
            request,
            std::bind(&::bosdyn::api::graph_nav::GraphNavRecordingService::Stub::AsyncStopRecording,
                      m_stub.get(), _1, _2, _3),
            std::bind(&GraphNavRecordingClient::OnStopRecordingComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

StopRecordingResultType GraphNavRecordingClient::StopRecording(
    ::bosdyn::api::graph_nav::StopRecordingRequest& request, const RPCParameters& parameters) {
    return StopRecordingAsync(request, parameters).get();
}

void GraphNavRecordingClient::OnStopRecordingComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::StopRecordingRequest& request,
    ::bosdyn::api::graph_nav::StopRecordingResponse&& response, const grpc::Status& status,
    std::promise<StopRecordingResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseWithLeaseAndGetFinalStatus<::bosdyn::api::graph_nav::StopRecordingResponse>(
            status, response, response.status(), m_lease_wallet.get());

    promise.set_value(
        {ret_status, std::move(response)});
}

std::shared_future<CreateWaypointResultType> GraphNavRecordingClient::CreateWaypointAsync(
    ::bosdyn::api::graph_nav::CreateWaypointRequest& request, const RPCParameters& parameters) {
    std::promise<CreateWaypointResultType> response;
    std::shared_future<CreateWaypointResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");
    // Run a lease processor function to attempt to automatically apply a lease to the request if
    // a lease is not already set.
    auto lease_status = ProcessRequestWithLease(&request, m_lease_wallet.get(),
                                                ::bosdyn::client::kBodyResource);
    if (!lease_status) {
        // Failed to set a lease with the lease wallet. Return early since the request will fail without a lease.
        response.set_value({lease_status, {}});
        return future;
    }

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::graph_nav::CreateWaypointRequest,
                          ::bosdyn::api::graph_nav::CreateWaypointResponse,
                          ::bosdyn::api::graph_nav::CreateWaypointResponse>(
            request,
            std::bind(&::bosdyn::api::graph_nav::GraphNavRecordingService::Stub::AsyncCreateWaypoint,
                      m_stub.get(), _1, _2, _3),
            std::bind(&GraphNavRecordingClient::OnCreateWaypointComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

CreateWaypointResultType GraphNavRecordingClient::CreateWaypoint(
    ::bosdyn::api::graph_nav::CreateWaypointRequest& request, const RPCParameters& parameters) {
    return CreateWaypointAsync(request, parameters).get();
}

void GraphNavRecordingClient::OnCreateWaypointComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::CreateWaypointRequest& request,
    ::bosdyn::api::graph_nav::CreateWaypointResponse&& response, const grpc::Status& status,
    std::promise<CreateWaypointResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseWithLeaseAndGetFinalStatus<::bosdyn::api::graph_nav::CreateWaypointResponse>(
            status, response, response.status(), m_lease_wallet.get());

    promise.set_value(
        {ret_status, std::move(response)});
}

std::shared_future<SetRecordingEnvironmentResultType>
GraphNavRecordingClient::SetRecordingEnvironmentAsync(
    ::bosdyn::api::graph_nav::SetRecordingEnvironmentRequest& request, const RPCParameters& parameters) {
    std::promise<SetRecordingEnvironmentResultType> response;
    std::shared_future<SetRecordingEnvironmentResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");
    // Run a lease processor function to attempt to automatically apply a lease to the request if
    // a lease is not already set.
    auto lease_status = ProcessRequestWithLease(&request, m_lease_wallet.get(),
                                                ::bosdyn::client::kBodyResource);
    if (!lease_status) {
        // Failed to set a lease with the lease wallet. Return early since the request will fail without a lease.
        response.set_value({lease_status, {}});
        return future;
    }

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::graph_nav::SetRecordingEnvironmentRequest,
                          ::bosdyn::api::graph_nav::SetRecordingEnvironmentResponse,
                          ::bosdyn::api::graph_nav::SetRecordingEnvironmentResponse>(
            request,
            std::bind(&::bosdyn::api::graph_nav::GraphNavRecordingService::Stub::AsyncSetRecordingEnvironment,
                      m_stub.get(), _1, _2, _3),
            std::bind(&GraphNavRecordingClient::OnSetRecordingEnvironmentComplete, this, _1, _2, _3,
                      _4, _5),
            std::move(response), parameters);

    return future;
}

SetRecordingEnvironmentResultType GraphNavRecordingClient::SetRecordingEnvironment(
    ::bosdyn::api::graph_nav::SetRecordingEnvironmentRequest& request, const RPCParameters& parameters) {
    return SetRecordingEnvironmentAsync(request, parameters).get();
}

void GraphNavRecordingClient::OnSetRecordingEnvironmentComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::SetRecordingEnvironmentRequest& request,
    ::bosdyn::api::graph_nav::SetRecordingEnvironmentResponse&& response, const grpc::Status& status,
    std::promise<SetRecordingEnvironmentResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseWithLeaseAndGetFinalStatus<::bosdyn::api::graph_nav::SetRecordingEnvironmentResponse>(
            status, response, SDKErrorCode::Success, m_lease_wallet.get());

    promise.set_value(
        {ret_status, std::move(response)});
}

std::shared_future<CreateEdgeResultType> GraphNavRecordingClient::CreateEdgeAsync(
    ::bosdyn::api::graph_nav::CreateEdgeRequest& request, const RPCParameters& parameters) {
    std::promise<CreateEdgeResultType> response;
    std::shared_future<CreateEdgeResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");
    // Run a lease processor function to attempt to automatically apply a lease to the request if
    // a lease is not already set.
    auto lease_status = ProcessRequestWithLease(&request, m_lease_wallet.get(),
                                                ::bosdyn::client::kBodyResource);
    if (!lease_status) {
        // Failed to set a lease with the lease wallet. Return early since the request will fail without a lease.
        response.set_value({lease_status, {}});
        return future;
    }

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::graph_nav::CreateEdgeRequest, ::bosdyn::api::graph_nav::CreateEdgeResponse,
                          ::bosdyn::api::graph_nav::CreateEdgeResponse>(
            request,
            std::bind(&::bosdyn::api::graph_nav::GraphNavRecordingService::Stub::AsyncCreateEdge,
                      m_stub.get(), _1, _2, _3),
            std::bind(&GraphNavRecordingClient::OnCreateEdgeComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

CreateEdgeResultType GraphNavRecordingClient::CreateEdge(
    ::bosdyn::api::graph_nav::CreateEdgeRequest& request, const RPCParameters& parameters) {
    return CreateEdgeAsync(request, parameters).get();
}

void GraphNavRecordingClient::OnCreateEdgeComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::CreateEdgeRequest& request,
    ::bosdyn::api::graph_nav::CreateEdgeResponse&& response, const grpc::Status& status,
    std::promise<CreateEdgeResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseWithLeaseAndGetFinalStatus<::bosdyn::api::graph_nav::CreateEdgeResponse>(
            status, response, SDKErrorCode::Success, m_lease_wallet.get());

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<GetRecordStatusResultType> GraphNavRecordingClient::GetRecordStatusAsync(
    const RPCParameters& parameters) {
    std::promise<GetRecordStatusResultType> response;
    std::shared_future<GetRecordStatusResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::graph_nav::GetRecordStatusRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::graph_nav::GetRecordStatusRequest,
                          ::bosdyn::api::graph_nav::GetRecordStatusResponse,
                          ::bosdyn::api::graph_nav::GetRecordStatusResponse>(
            request,
            std::bind(&::bosdyn::api::graph_nav::GraphNavRecordingService::Stub::AsyncGetRecordStatus,
                      m_stub.get(), _1, _2, _3),
            std::bind(&GraphNavRecordingClient::OnGetRecordStatusComplete, this, _1, _2, _3, _4,
                      _5),
            std::move(response), parameters);

    return future;
}

GetRecordStatusResultType GraphNavRecordingClient::GetRecordStatus(
    const RPCParameters& parameters) {
    return GetRecordStatusAsync(parameters).get();
}

void GraphNavRecordingClient::OnGetRecordStatusComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::GetRecordStatusRequest& request,
    ::bosdyn::api::graph_nav::GetRecordStatusResponse&& response, const grpc::Status& status,
    std::promise<GetRecordStatusResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::graph_nav::GetRecordStatusResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value(
        {ret_status, std::move(response)});
}

ServiceClient::QualityOfService GraphNavRecordingClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void GraphNavRecordingClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::graph_nav::GraphNavRecordingService::Stub(channel));
}

void GraphNavRecordingClient::UpdateServiceFrom(RequestProcessorChain& request_processor_chain,
                                                ResponseProcessorChain& response_processor_chain,
                                                const std::shared_ptr<LeaseWallet>& lease_wallet) {
    // Set the lease wallet.
    m_lease_wallet = lease_wallet;

    // Set the request and response processors.
    m_request_processor_chain = request_processor_chain;
    m_response_processor_chain = response_processor_chain;
}

}  // namespace client

}  // namespace bosdyn
