/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/graph_nav/graph_nav_client.h"
#include "bosdyn/client/data_chunk/data_chunking.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"
#include "bosdyn/client/lease/lease_processors.h"
#include "bosdyn/client/lease/lease_resources.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* GraphNavClient::s_default_service_name = "graph-nav-service";

const char* GraphNavClient::s_service_type = "bosdyn.api.graph_nav.GraphNavService";

std::shared_future<SetLocalizationResultType> GraphNavClient::SetLocalizationAsync(
    ::bosdyn::api::graph_nav::SetLocalizationRequest& request,
    const RPCParameters& parameters) {
    std::promise<SetLocalizationResultType> response;
    std::shared_future<SetLocalizationResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::graph_nav::SetLocalizationRequest,
                          ::bosdyn::api::graph_nav::SetLocalizationResponse,
                          ::bosdyn::api::graph_nav::SetLocalizationResponse>(
            request,
            std::bind(&::bosdyn::api::graph_nav::GraphNavService::Stub::AsyncSetLocalization, m_stub.get(),
                      _1, _2, _3),
            std::bind(&GraphNavClient::OnSetLocalizationComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

SetLocalizationResultType GraphNavClient::SetLocalization(
    ::bosdyn::api::graph_nav::SetLocalizationRequest& request,
    const RPCParameters& parameters) {
    return SetLocalizationAsync(request, parameters).get();
}

void GraphNavClient::OnSetLocalizationComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::SetLocalizationRequest& request,
    ::bosdyn::api::graph_nav::SetLocalizationResponse&& response, const grpc::Status& status,
    std::promise<SetLocalizationResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::graph_nav::SetLocalizationResponse>(
        status, response, response.status());

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<NavigateRouteResultType> GraphNavClient::NavigateRouteAsync(
    ::bosdyn::api::graph_nav::NavigateRouteRequest& request,
    const RPCParameters& parameters) {
    std::promise<NavigateRouteResultType> response;
    std::shared_future<NavigateRouteResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    // Run a lease processor function to attempt to automatically apply the necessary leases to the
    // request if a lease is not already in the request.
    auto lease_status = ProcessRequestWithMultipleLeases(&request, m_lease_wallet.get(),
                                                         {::bosdyn::client::kBodyResource});
    if (!lease_status) {
        // Failed to set a lease with the lease wallet. Return early since the request will fail
        // without a lease.
        response.set_value({lease_status, {}});
        return future;
    }

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::graph_nav::NavigateRouteRequest,
                          ::bosdyn::api::graph_nav::NavigateRouteResponse,
                          ::bosdyn::api::graph_nav::NavigateRouteResponse>(
            request,
            std::bind(&::bosdyn::api::graph_nav::GraphNavService::Stub::AsyncNavigateRoute, m_stub.get(), _1,
                      _2, _3),
            std::bind(&GraphNavClient::OnNavigateRouteComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

NavigateRouteResultType GraphNavClient::NavigateRoute(
    ::bosdyn::api::graph_nav::NavigateRouteRequest& request,
    const RPCParameters& parameters) {
    return NavigateRouteAsync(request, parameters).get();
}

void GraphNavClient::OnNavigateRouteComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::NavigateRouteRequest& request,
    ::bosdyn::api::graph_nav::NavigateRouteResponse&& response, const grpc::Status& status,
    std::promise<NavigateRouteResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseWithMultiLeaseAndGetFinalStatus<::bosdyn::api::graph_nav::NavigateRouteResponse>(
            status, response, response.status(), m_lease_wallet.get());

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<NavigateToResultType> GraphNavClient::NavigateToAsync(
    ::bosdyn::api::graph_nav::NavigateToRequest& request,
    const RPCParameters& parameters) {
    std::promise<NavigateToResultType> response;
    std::shared_future<NavigateToResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    // Run a lease processor function to attempt to automatically apply the necessary leases to the
    // request if a lease is not already in the request.
    auto lease_status = ProcessRequestWithMultipleLeases(&request, m_lease_wallet.get(),
                                                         {::bosdyn::client::kBodyResource});
    if (!lease_status) {
        // Failed to set a lease with the lease wallet. Return early since the request will fail
        // without a lease.
        response.set_value({lease_status, {}});
        return future;
    }

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::graph_nav::NavigateToRequest, ::bosdyn::api::graph_nav::NavigateToResponse,
                          ::bosdyn::api::graph_nav::NavigateToResponse>(
            request,
            std::bind(&::bosdyn::api::graph_nav::GraphNavService::Stub::AsyncNavigateTo, m_stub.get(), _1, _2,
                      _3),
            std::bind(&GraphNavClient::OnNavigateToComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

NavigateToResultType GraphNavClient::NavigateTo(
    ::bosdyn::api::graph_nav::NavigateToRequest& request,
    const RPCParameters& parameters) {
    return NavigateToAsync(request, parameters).get();
}

void GraphNavClient::OnNavigateToComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::NavigateToRequest& request,
    ::bosdyn::api::graph_nav::NavigateToResponse&& response, const grpc::Status& status,
    std::promise<NavigateToResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseWithMultiLeaseAndGetFinalStatus<::bosdyn::api::graph_nav::NavigateToResponse>(
            status, response, response.status(), m_lease_wallet.get());

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<NavigationFeedbackResultType> GraphNavClient::NavigationFeedbackAsync(
    const unsigned int command_id, const RPCParameters& parameters) {
    std::promise<NavigationFeedbackResultType> response;
    std::shared_future<NavigationFeedbackResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::graph_nav::NavigationFeedbackRequest request;
    request.set_command_id(command_id);

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::graph_nav::NavigationFeedbackRequest,
                          ::bosdyn::api::graph_nav::NavigationFeedbackResponse,
                          ::bosdyn::api::graph_nav::NavigationFeedbackResponse>(
            request,
            std::bind(&::bosdyn::api::graph_nav::GraphNavService::Stub::AsyncNavigationFeedback, m_stub.get(),
                      _1, _2, _3),
            std::bind(&GraphNavClient::OnNavigationFeedbackComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

NavigationFeedbackResultType GraphNavClient::NavigationFeedback(
    const unsigned int command_id, const RPCParameters& parameters) {
    return NavigationFeedbackAsync(command_id, parameters).get();
}

void GraphNavClient::OnNavigationFeedbackComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::NavigationFeedbackRequest& request,
    ::bosdyn::api::graph_nav::NavigationFeedbackResponse&& response, const grpc::Status& status,
    std::promise<NavigationFeedbackResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseAndGetFinalStatus<::bosdyn::api::graph_nav::NavigationFeedbackResponse>(
            status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<GetLocalizationStateResultType> GraphNavClient::GetLocalizationStateAsync(
    ::bosdyn::api::graph_nav::GetLocalizationStateRequest& request,
    const RPCParameters& parameters) {
    std::promise<GetLocalizationStateResultType> response;
    std::shared_future<GetLocalizationStateResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::graph_nav::GetLocalizationStateRequest,
                          ::bosdyn::api::graph_nav::GetLocalizationStateResponse,
                          ::bosdyn::api::graph_nav::GetLocalizationStateResponse>(
            request,
            std::bind(&::bosdyn::api::graph_nav::GraphNavService::Stub::AsyncGetLocalizationState,
                      m_stub.get(), _1, _2, _3),
            std::bind(&GraphNavClient::OnGetLocalizationStateComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

GetLocalizationStateResultType GraphNavClient::GetLocalizationState(
    ::bosdyn::api::graph_nav::GetLocalizationStateRequest& request,
    const RPCParameters& parameters) {
    return GetLocalizationStateAsync(request, parameters).get();
}

void GraphNavClient::OnGetLocalizationStateComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::GetLocalizationStateRequest& request,
    ::bosdyn::api::graph_nav::GetLocalizationStateResponse&& response, const grpc::Status& status,
    std::promise<GetLocalizationStateResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseAndGetFinalStatus<::bosdyn::api::graph_nav::GetLocalizationStateResponse>(
            status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<ClearGraphResultType> GraphNavClient::ClearGraphAsync(
    const RPCParameters& parameters) {
    std::promise<ClearGraphResultType> response;
    std::shared_future<ClearGraphResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");
    ::bosdyn::api::graph_nav::ClearGraphRequest request;
    // Run a lease processor function to attempt to automatically apply a lease to the request if
    // a lease is not already in the request.
    auto lease_status = ProcessRequestWithLease(&request, m_lease_wallet.get(),
                                                ::bosdyn::client::kBodyResource);
    if (!lease_status) {
        // Failed to set a lease with the lease wallet. Return early since the request will fail without a lease.
        response.set_value({lease_status, {}});
        return future;
    }

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::graph_nav::ClearGraphRequest, ::bosdyn::api::graph_nav::ClearGraphResponse,
                          ::bosdyn::api::graph_nav::ClearGraphResponse>(
            request,
            std::bind(&::bosdyn::api::graph_nav::GraphNavService::Stub::AsyncClearGraph, m_stub.get(), _1, _2,
                      _3),
            std::bind(&GraphNavClient::OnClearGraphComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

ClearGraphResultType GraphNavClient::ClearGraph(
    const RPCParameters& parameters) {
    return ClearGraphAsync(parameters).get();
}

void GraphNavClient::OnClearGraphComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::ClearGraphRequest& request,
    ::bosdyn::api::graph_nav::ClearGraphResponse&& response, const grpc::Status& status,
    std::promise<ClearGraphResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseWithLeaseAndGetFinalStatus<::bosdyn::api::graph_nav::ClearGraphResponse>(
        status, response, response.status(), m_lease_wallet.get());

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<DownloadGraphResultType> GraphNavClient::DownloadGraphAsync(
    const RPCParameters& parameters) {
    std::promise<DownloadGraphResultType> response;
    std::shared_future<DownloadGraphResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::graph_nav::DownloadGraphRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::graph_nav::DownloadGraphRequest,
                          ::bosdyn::api::graph_nav::DownloadGraphResponse,
                          ::bosdyn::api::graph_nav::DownloadGraphResponse>(
            request,
            std::bind(&::bosdyn::api::graph_nav::GraphNavService::Stub::AsyncDownloadGraph, m_stub.get(), _1,
                      _2, _3),
            std::bind(&GraphNavClient::OnDownloadGraphComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

DownloadGraphResultType GraphNavClient::DownloadGraph(
    const RPCParameters& parameters) {
    return DownloadGraphAsync(parameters).get();
}

void GraphNavClient::OnDownloadGraphComplete(MessagePumpCallBase* call,
                                             const ::bosdyn::api::graph_nav::DownloadGraphRequest& request,
                                             ::bosdyn::api::graph_nav::DownloadGraphResponse&& response,
                                             const grpc::Status& status,
                                             std::promise<DownloadGraphResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::graph_nav::DownloadGraphResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<UploadGraphResultType> GraphNavClient::UploadGraphAsync(
    ::bosdyn::api::graph_nav::UploadGraphRequest& request,
    const RPCParameters& parameters) {
    std::promise<UploadGraphResultType> response;
    std::shared_future<UploadGraphResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    // Run a lease processor function to attempt to automatically apply a lease to the request if
    // a lease is not already in the request.
    auto lease_status = ProcessRequestWithLease(&request, m_lease_wallet.get(),
                                                ::bosdyn::client::kBodyResource);
    if (!lease_status) {
        // Failed to set a lease with the lease wallet. Return early since the request will fail without a lease.
        response.set_value({lease_status, {}});
        return future;
    }

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::graph_nav::UploadGraphRequest, ::bosdyn::api::graph_nav::UploadGraphResponse,
                          ::bosdyn::api::graph_nav::UploadGraphResponse>(
            request,
            std::bind(&::bosdyn::api::graph_nav::GraphNavService::Stub::AsyncUploadGraph, m_stub.get(), _1,
                      _2, _3),
            std::bind(&GraphNavClient::OnUploadGraphComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

UploadGraphResultType GraphNavClient::UploadGraph(
    ::bosdyn::api::graph_nav::UploadGraphRequest& request,
    const RPCParameters& parameters) {
    return UploadGraphAsync(request, parameters).get();
}

void GraphNavClient::OnUploadGraphComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::UploadGraphRequest& request,
    ::bosdyn::api::graph_nav::UploadGraphResponse&& response, const grpc::Status& status,
    std::promise<UploadGraphResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseWithLeaseAndGetFinalStatus<::bosdyn::api::graph_nav::UploadGraphResponse>(
            status, response, response.status(), m_lease_wallet.get());

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<UploadWaypointSnapshotResultType> GraphNavClient::UploadWaypointSnapshotAsync(
    ::bosdyn::api::graph_nav::WaypointSnapshot& input_request,
    const RPCParameters& parameters) {
    std::promise<UploadWaypointSnapshotResultType> response;
    std::shared_future<UploadWaypointSnapshotResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    std::vector<::bosdyn::api::DataChunk> chunks;
    std::vector<::bosdyn::api::graph_nav::UploadWaypointSnapshotRequest> requests;

    ::bosdyn::common::Status status = MessageToDataChunks<::bosdyn::api::graph_nav::WaypointSnapshot>(input_request, &chunks);
    if (!status) {
        response.set_value({status, {}});
        return future;
    }

    for (auto& chunk : chunks) {
        ::bosdyn::api::graph_nav::UploadWaypointSnapshotRequest request;

        auto request_chunk = request.mutable_chunk();
        *request_chunk = std::move(chunk);

        // For each request, apply the lease processor to automatically include a lease.
        auto lease_status = ProcessRequestWithLease(&request, m_lease_wallet.get(),
                                                    ::bosdyn::client::kBodyResource);
        if (!lease_status) {
            // Failed to set a lease with the lease wallet. Return early since the request will fail without a lease.
            response.set_value({lease_status, {}});
            return future;
        }

        requests.emplace_back(std::move(request));
    }

    MessagePumpCallBase* one_time =
        InitiateRequestStreamAsyncCall<::bosdyn::api::graph_nav::UploadWaypointSnapshotRequest,
                                       ::bosdyn::api::graph_nav::UploadWaypointSnapshotResponse,
                                       ::bosdyn::api::graph_nav::UploadWaypointSnapshotResponse>(
        std::move(requests),
        std::bind(&::bosdyn::api::graph_nav::GraphNavService::Stub::AsyncUploadWaypointSnapshot, m_stub.get(),
                  _1, _2, _3, _4),
        std::bind(&GraphNavClient::OnUploadWaypointSnapshotComplete, this, _1, _2, _3, _4, _5),
        std::move(response), parameters);

    return future;
}

UploadWaypointSnapshotResultType GraphNavClient::UploadWaypointSnapshot(
    ::bosdyn::api::graph_nav::WaypointSnapshot& input_request,
    const RPCParameters& parameters) {
    return UploadWaypointSnapshotAsync(input_request, parameters).get();
}

void GraphNavClient::OnUploadWaypointSnapshotComplete(
    MessagePumpCallBase* call,
    const std::vector<::bosdyn::api::graph_nav::UploadWaypointSnapshotRequest>&& request,
    ::bosdyn::api::graph_nav::UploadWaypointSnapshotResponse&& response, const grpc::Status& status,
    std::promise<UploadWaypointSnapshotResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseWithLeaseAndGetFinalStatus<::bosdyn::api::graph_nav::UploadWaypointSnapshotResponse>(
            status, response, response.status(), m_lease_wallet.get());

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<UploadEdgeSnapshotResultType> GraphNavClient::UploadEdgeSnapshotAsync(
    ::bosdyn::api::graph_nav::EdgeSnapshot& input_request,
    const RPCParameters& parameters) {
    std::promise<UploadEdgeSnapshotResultType> response;
    std::shared_future<UploadEdgeSnapshotResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    std::vector<::bosdyn::api::DataChunk> chunks;
    std::vector<::bosdyn::api::graph_nav::UploadEdgeSnapshotRequest> requests;
    ::bosdyn::common::Status status = MessageToDataChunks<::bosdyn::api::graph_nav::EdgeSnapshot>(input_request, &chunks);
    if (!status) {
        response.set_value({status, {}});
        return future;
    }

    for (auto& chunk : chunks) {
        ::bosdyn::api::graph_nav::UploadEdgeSnapshotRequest request;

        auto request_chunk = request.mutable_chunk();
        *request_chunk = std::move(chunk);

        // For each request, apply the lease processor to automatically include a lease.
        auto lease_status = ProcessRequestWithLease(&request, m_lease_wallet.get(),
                                                    ::bosdyn::client::kBodyResource);
        if (!lease_status) {
            // Failed to set a lease with the lease wallet. Return early since the request will fail without a lease.
            response.set_value({lease_status, {}});
            return future;
        }

        requests.emplace_back(std::move(request));
    }

    MessagePumpCallBase* one_time =
        InitiateRequestStreamAsyncCall<::bosdyn::api::graph_nav::UploadEdgeSnapshotRequest,
                                       ::bosdyn::api::graph_nav::UploadEdgeSnapshotResponse,
                                       ::bosdyn::api::graph_nav::UploadEdgeSnapshotResponse>(
            std::move(requests),
            std::bind(&::bosdyn::api::graph_nav::GraphNavService::Stub::AsyncUploadEdgeSnapshot, m_stub.get(),
                      _1, _2, _3, _4),
            std::bind(&GraphNavClient::OnUploadEdgeSnapshotComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

UploadEdgeSnapshotResultType GraphNavClient::UploadEdgeSnapshot(
    ::bosdyn::api::graph_nav::EdgeSnapshot& input_request,
    const RPCParameters& parameters) {
    return UploadEdgeSnapshotAsync(input_request, parameters).get();
}

void GraphNavClient::OnUploadEdgeSnapshotComplete(
    MessagePumpCallBase* call,
    const std::vector<::bosdyn::api::graph_nav::UploadEdgeSnapshotRequest>&& request,
    ::bosdyn::api::graph_nav::UploadEdgeSnapshotResponse&& response, const grpc::Status& status,
    std::promise<UploadEdgeSnapshotResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseWithLeaseAndGetFinalStatus<::bosdyn::api::graph_nav::UploadEdgeSnapshotResponse>(
            status, response, SDKErrorCode::Success, m_lease_wallet.get());

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<DownloadWaypointSnapshotResultType>
GraphNavClient::DownloadWaypointSnapshotAsync(
    ::bosdyn::api::graph_nav::DownloadWaypointSnapshotRequest& request,
    const RPCParameters& parameters) {
    std::promise<DownloadWaypointSnapshotResultType> response;
    std::shared_future<DownloadWaypointSnapshotResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateResponseStreamAsyncCall<::bosdyn::api::graph_nav::DownloadWaypointSnapshotRequest,
                                        ::bosdyn::api::graph_nav::DownloadWaypointSnapshotResponse,
                                        ::bosdyn::api::graph_nav::WaypointSnapshot>(
            request,
            std::bind(&::bosdyn::api::graph_nav::GraphNavService::Stub::AsyncDownloadWaypointSnapshot,
                      m_stub.get(), _1, _2, _3, _4),
            std::bind(&GraphNavClient::OnDownloadWaypointSnapshotComplete, this, _1, _2, _3, _4,
                      _5),
            std::move(response), parameters);

    return future;
}

DownloadWaypointSnapshotResultType GraphNavClient::DownloadWaypointSnapshot(
    ::bosdyn::api::graph_nav::DownloadWaypointSnapshotRequest& request,
    const RPCParameters& parameters) {
    return DownloadWaypointSnapshotAsync(request, parameters).get();
}

void GraphNavClient::OnDownloadWaypointSnapshotComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::DownloadWaypointSnapshotRequest& request,
    std::vector<::bosdyn::api::graph_nav::DownloadWaypointSnapshotResponse>&& responses,
    const grpc::Status& status, std::promise<DownloadWaypointSnapshotResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseVectorAndGetFinalStatus<::bosdyn::api::graph_nav::DownloadWaypointSnapshotResponse>(
            status, responses);
    if (!ret_status) {
        promise.set_value({ret_status, {}});
        return;
    }

    if (responses.empty()) {
        promise.set_value({::bosdyn::common::Status(SDKErrorCode::GenericSDKError,
                                  "Empty vector of DownloadWaypointSnapshotResponse received"),
                          {}});
        return;
    }

    std::string waypoint_snapshot_id = responses[0].waypoint_snapshot_id();
    ::bosdyn::api::graph_nav::WaypointSnapshot full_response;
    std::vector<const ::bosdyn::api::DataChunk*> data_chunks;

    for (auto& response : responses) {
        data_chunks.push_back(&response.chunk());
        if (response.waypoint_snapshot_id() != waypoint_snapshot_id) {
            promise.set_value(
                {::bosdyn::common::Status(
                     SDKErrorCode::GenericSDKError,
                     "Multiple waypoint IDs in DownloadWaypointSnapshotResponse vector received"),
                 {}});
            return;
        }
    }

    promise.set_value(MessageFromDataChunks<::bosdyn::api::graph_nav::WaypointSnapshot>(data_chunks));
}

std::shared_future<DownloadEdgeSnapshotResultType> GraphNavClient::DownloadEdgeSnapshotAsync(
    ::bosdyn::api::graph_nav::DownloadEdgeSnapshotRequest& request,
    const RPCParameters& parameters) {
    std::promise<DownloadEdgeSnapshotResultType> response;
    std::shared_future<DownloadEdgeSnapshotResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateResponseStreamAsyncCall<::bosdyn::api::graph_nav::DownloadEdgeSnapshotRequest,
                                        ::bosdyn::api::graph_nav::DownloadEdgeSnapshotResponse,
                                        ::bosdyn::api::graph_nav::EdgeSnapshot>(
            request,
            std::bind(&::bosdyn::api::graph_nav::GraphNavService::Stub::AsyncDownloadEdgeSnapshot,
                      m_stub.get(), _1, _2, _3, _4),
            std::bind(&GraphNavClient::OnDownloadEdgeSnapshotComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

DownloadEdgeSnapshotResultType GraphNavClient::DownloadEdgeSnapshot(
    ::bosdyn::api::graph_nav::DownloadEdgeSnapshotRequest& request,
    const RPCParameters& parameters) {
    return DownloadEdgeSnapshotAsync(request, parameters).get();
}

void GraphNavClient::OnDownloadEdgeSnapshotComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::DownloadEdgeSnapshotRequest& request,
    std::vector<::bosdyn::api::graph_nav::DownloadEdgeSnapshotResponse>&& responses,
    const grpc::Status& status, std::promise<DownloadEdgeSnapshotResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseVectorAndGetFinalStatus<::bosdyn::api::graph_nav::DownloadEdgeSnapshotResponse>(
            status, responses);
    if (!ret_status) {
        promise.set_value({ret_status, {}});
        return;
    }

    if (responses.empty()) {
        promise.set_value({::bosdyn::common::Status(SDKErrorCode::GenericSDKError,
                                  "Empty vector of DownloadEdgeSnapshotResponse received"),
                           {}});
        return;
    }

    std::string edge_snapshot_id = responses[0].edge_snapshot_id();
    ::bosdyn::api::graph_nav::WaypointSnapshot full_response;
    std::vector<const ::bosdyn::api::DataChunk*> data_chunks;

    for (auto& response : responses) {
        data_chunks.push_back(&response.chunk());
        if (response.edge_snapshot_id() != edge_snapshot_id) {
            promise.set_value(
                {::bosdyn::common::Status(SDKErrorCode::GenericSDKError,
                        "Multiple edge IDs in DownloadEdgeSnapshotResponse vector received"),
                 {}});
            return;
        }
    }

    promise.set_value(MessageFromDataChunks<::bosdyn::api::graph_nav::EdgeSnapshot>(data_chunks));
}


ServiceClient::QualityOfService GraphNavClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void GraphNavClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::graph_nav::GraphNavService::Stub(channel));
}

void GraphNavClient::UpdateServiceFrom(RequestProcessorChain& request_processor_chain,
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
