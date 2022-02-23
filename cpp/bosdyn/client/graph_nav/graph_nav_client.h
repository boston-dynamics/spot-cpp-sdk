/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/graph_nav/graph_nav_service.grpc.pb.h>
#include <bosdyn/api/graph_nav/graph_nav_service.pb.h>

#include <future>

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/common/status.h"
#include "bosdyn/client/graph_nav/graph_nav_error_codes.h"

namespace bosdyn {

namespace client {

// Return type for the SetLocalization method.
typedef Result<::bosdyn::api::graph_nav::SetLocalizationResponse> SetLocalizationResultType;

// Return type for the NavigateRoute method.
typedef Result<::bosdyn::api::graph_nav::NavigateRouteResponse> NavigateRouteResultType;

// Return type for the NavigateTo method.
typedef Result<::bosdyn::api::graph_nav::NavigateToResponse> NavigateToResultType;

// Return type for the NavigationFeedback method.
typedef Result<::bosdyn::api::graph_nav::NavigationFeedbackResponse> NavigationFeedbackResultType;

// Return type for the GetLocalizationState method.
typedef Result<::bosdyn::api::graph_nav::GetLocalizationStateResponse>
    GetLocalizationStateResultType;

// Return type for the ClearGraph method.
typedef Result<::bosdyn::api::graph_nav::ClearGraphResponse> ClearGraphResultType;

// Return type for the DownloadGraph method.
typedef Result<::bosdyn::api::graph_nav::DownloadGraphResponse> DownloadGraphResultType;

// Return type for the UploadGraph method.
typedef Result<::bosdyn::api::graph_nav::UploadGraphResponse> UploadGraphResultType;

// Return type for the UploadWaypointSnapshot method.
typedef Result<::bosdyn::api::graph_nav::UploadWaypointSnapshotResponse> UploadWaypointSnapshotResultType;

// Return type for the UploadEdgeSnapshot method.
typedef Result<::bosdyn::api::graph_nav::UploadEdgeSnapshotResponse> UploadEdgeSnapshotResultType;

// Return type for the DownloadWaypointSnapshot method.
typedef Result<::bosdyn::api::graph_nav::WaypointSnapshot> DownloadWaypointSnapshotResultType;

// Return type for the DownloadEdgeSnapshot method.
typedef Result<::bosdyn::api::graph_nav::EdgeSnapshot> DownloadEdgeSnapshotResultType;


class GraphNavClient : public ServiceClient {
 public:
    // Constructor for the GraphNav client, which can be used to make RPC requests for GraphNav
    // commands.
    GraphNavClient() = default;

    // Destructor for the GraphNav client.
    ~GraphNavClient() = default;

    // Asynchronous method to execute a SetLocalization request.
    std::shared_future<SetLocalizationResultType> SetLocalizationAsync(
        ::bosdyn::api::graph_nav::SetLocalizationRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to execute a SetLocalization request.
    SetLocalizationResultType SetLocalization(
        ::bosdyn::api::graph_nav::SetLocalizationRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to execute a NavigateRoute request.
    std::shared_future<NavigateRouteResultType> NavigateRouteAsync(
        ::bosdyn::api::graph_nav::NavigateRouteRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to execute a NavigateRoute request.
    NavigateRouteResultType NavigateRoute(
        ::bosdyn::api::graph_nav::NavigateRouteRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to execute a NavigateTo request.
    std::shared_future<NavigateToResultType> NavigateToAsync(
        ::bosdyn::api::graph_nav::NavigateToRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to execute a NavigateTo request.
    NavigateToResultType NavigateTo(
        ::bosdyn::api::graph_nav::NavigateToRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to execute a NavigationFeedback request.
    std::shared_future<NavigationFeedbackResultType> NavigationFeedbackAsync(
        unsigned int command_id,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to execute a NavigationFeedback request.
    NavigationFeedbackResultType NavigationFeedback(
        unsigned int command_id,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to execute a GetLocalizationState request.
    std::shared_future<GetLocalizationStateResultType> GetLocalizationStateAsync(
        ::bosdyn::api::graph_nav::GetLocalizationStateRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to execute a GetLocalizationState request.
    GetLocalizationStateResultType GetLocalizationState(
        ::bosdyn::api::graph_nav::GetLocalizationStateRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to execute a ClearGraph request.
    std::shared_future<ClearGraphResultType> ClearGraphAsync(
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to execute a ClearGraph request.
    ClearGraphResultType ClearGraph(
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to execute a DownloadGraph request.
    std::shared_future<DownloadGraphResultType> DownloadGraphAsync(
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to execute a DownloadGraph request.
    DownloadGraphResultType DownloadGraph(
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to execute a UploadGraph request.
    std::shared_future<UploadGraphResultType> UploadGraphAsync(
        ::bosdyn::api::graph_nav::UploadGraphRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to execute a UploadGraph request.
    UploadGraphResultType UploadGraph(
        ::bosdyn::api::graph_nav::UploadGraphRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to execute a UploadWaypointSnapshot request.
    std::shared_future<UploadWaypointSnapshotResultType> UploadWaypointSnapshotAsync(
        ::bosdyn::api::graph_nav::WaypointSnapshot& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to execute a UploadWaypointSnapshot request.
    UploadWaypointSnapshotResultType UploadWaypointSnapshot(
        ::bosdyn::api::graph_nav::WaypointSnapshot& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to execute a UploadEdgeSnapshot request.
    std::shared_future<UploadEdgeSnapshotResultType> UploadEdgeSnapshotAsync(
        ::bosdyn::api::graph_nav::EdgeSnapshot& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to execute a UploadEdgeSnapshot request.
    UploadEdgeSnapshotResultType UploadEdgeSnapshot(
        ::bosdyn::api::graph_nav::EdgeSnapshot& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to execute a DownloadWaypointSnapshot request. Only one response
    // message is returned with all the data chunks received from the service concatenated
    // together.
    std::shared_future<DownloadWaypointSnapshotResultType> DownloadWaypointSnapshotAsync(
        ::bosdyn::api::graph_nav::DownloadWaypointSnapshotRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to execute a DownloadWaypointSnapshot request. Only one response
    // message is returned with all the data chunks received from the service concatenated
    // together.
    DownloadWaypointSnapshotResultType DownloadWaypointSnapshot(
        ::bosdyn::api::graph_nav::DownloadWaypointSnapshotRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to execute a DownloadEdgeSnapshot request. Only one response
    // message is returned with all the data chunks received from the service concatenated
    // together.
    std::shared_future<DownloadEdgeSnapshotResultType> DownloadEdgeSnapshotAsync(
        ::bosdyn::api::graph_nav::DownloadEdgeSnapshotRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to execute a DownloadEdgeSnapshot request. Only one response
    // message is returned with all the data chunks received from the service concatenated
    // together.
    DownloadEdgeSnapshotResultType DownloadEdgeSnapshot(
        ::bosdyn::api::graph_nav::DownloadEdgeSnapshotRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Convenience methods to download an edge snapshot with just an id.
    std::shared_future<DownloadEdgeSnapshotResultType> DownloadEdgeSnapshotAsync(
        const std::string& edge_snapshot_id,
        const RPCParameters& parameters = RPCParameters()) {
        ::bosdyn::api::graph_nav::DownloadEdgeSnapshotRequest request;
        request.set_edge_snapshot_id(edge_snapshot_id);
        return DownloadEdgeSnapshotAsync(request, parameters);
    }

    DownloadEdgeSnapshotResultType DownloadEdgeSnapshot(
        const std::string& edge_snapshot_id,
        const RPCParameters& parameters = RPCParameters()) {
        ::bosdyn::api::graph_nav::DownloadEdgeSnapshotRequest request;
        request.set_edge_snapshot_id(edge_snapshot_id);
        return DownloadEdgeSnapshot(request, parameters);
    }


    // Sets the QualityOfService enum for the GraphNav client to be used for network selection
    // optimization.
    QualityOfService GetQualityOfService() const override;

    // Set the communication details for the GraphNav service, including the stub and processors
    // for requests/responses.
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;

    // Override the update from method to set the lease wallet for the GraphNav client
    // to be the same as the lease wallet of the robot object.
    void UpdateServiceFrom(RequestProcessorChain& request_processor_chain,
                           ResponseProcessorChain& response_processor_chain,
                           const std::shared_ptr<LeaseWallet>& lease_wallet) override;

    // Get the default service name the GraphNav service will be registered in the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the GraphNav service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // The stub for the GraphNav client to communicate it's rpc with (and pass to the robot).
    std::unique_ptr<::bosdyn::api::graph_nav::GraphNavService::Stub> m_stub;

    // Default service name for the GraphNav service.
    static const char* s_default_service_name;

    // Default service type for the GraphNav service.
    static const char* s_service_type;

    // Lease wallet for commands.
    std::shared_ptr<LeaseWallet> m_lease_wallet;

    // Callback that will return the SetLocalizationResponse message after SetLocalization rpc
    // returns to the client.
    void OnSetLocalizationComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::SetLocalizationRequest& request,
        ::bosdyn::api::graph_nav::SetLocalizationResponse&& response, const grpc::Status& status,
        std::promise<SetLocalizationResultType> promise);

    // Callback that will return the NavigateRouteResponse message after NavigateRoute rpc
    // returns to the client.
    void OnNavigateRouteComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::NavigateRouteRequest& request,
        ::bosdyn::api::graph_nav::NavigateRouteResponse&& response, const grpc::Status& status,
        std::promise<NavigateRouteResultType> promise);

    // Callback that will return the NavigateToResponse message after NavigateTo rpc
    // returns to the client.
    void OnNavigateToComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::NavigateToRequest& request,
        ::bosdyn::api::graph_nav::NavigateToResponse&& response, const grpc::Status& status,
        std::promise<NavigateToResultType> promise);

    // Callback that will return the NavigationFeedbackResponse message after NavigationFeedback rpc
    // returns to the client.
    void OnNavigationFeedbackComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::NavigationFeedbackRequest& request,
        ::bosdyn::api::graph_nav::NavigationFeedbackResponse&& response, const grpc::Status& status,
        std::promise<NavigationFeedbackResultType> promise);

    // Callback that will return the GetLocalizationStateResponse message after GetLocalizationState
    // rpc returns to the client.
    void OnGetLocalizationStateComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::GetLocalizationStateRequest& request,
        ::bosdyn::api::graph_nav::GetLocalizationStateResponse&& response, const grpc::Status& status,
        std::promise<GetLocalizationStateResultType> promise);

    // Callback that will return the ClearGraphResponse message after ClearGraph rpc
    // returns to the client.
    void OnClearGraphComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::ClearGraphRequest& request,
        ::bosdyn::api::graph_nav::ClearGraphResponse&& response, const grpc::Status& status,
        std::promise<ClearGraphResultType> promise);

    // Callback that will return the DownloadGraphResponse message after DownloadGraph rpc
    // returns to the client.
    void OnDownloadGraphComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::DownloadGraphRequest& request,
        ::bosdyn::api::graph_nav::DownloadGraphResponse&& response, const grpc::Status& status,
        std::promise<DownloadGraphResultType> promise);

    // Callback that will return the UploadGraphResponse message after UploadGraph rpc
    // returns to the client.
    void OnUploadGraphComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::UploadGraphRequest& request,
        ::bosdyn::api::graph_nav::UploadGraphResponse&& response, const grpc::Status& status,
        std::promise<UploadGraphResultType> promise);

    // Callback that will return the UploadWaypointSnapshotResponse message after
    // UploadWaypointSnapshot rpc returns to the client.
    void OnUploadWaypointSnapshotComplete(
        MessagePumpCallBase* call,
        const std::vector<::bosdyn::api::graph_nav::UploadWaypointSnapshotRequest>&& request,
        ::bosdyn::api::graph_nav::UploadWaypointSnapshotResponse&& response, const grpc::Status& status,
        std::promise<UploadWaypointSnapshotResultType> promise);

    // Callback that will return the UploadEdgeSnapshotResponse message after UploadEdgeSnapshot rpc
    // returns to the client.
    void OnUploadEdgeSnapshotComplete(
        MessagePumpCallBase* call,
        const std::vector<::bosdyn::api::graph_nav::UploadEdgeSnapshotRequest>&& request,
        ::bosdyn::api::graph_nav::UploadEdgeSnapshotResponse&& response, const grpc::Status& status,
        std::promise<UploadEdgeSnapshotResultType> promise);

    // Callback that will return the DownloadWaypointSnapshotResponse message after
    // DownloadWaypointSnapshot rpc returns to the client.
    void OnDownloadWaypointSnapshotComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::DownloadWaypointSnapshotRequest& request,
        std::vector<::bosdyn::api::graph_nav::DownloadWaypointSnapshotResponse>&& response,
        const grpc::Status& status, std::promise<DownloadWaypointSnapshotResultType> promise);

    // Callback that will return the DownloadEdgeSnapshotResponse message after DownloadEdgeSnapshot
    // rpc returns to the client.
    void OnDownloadEdgeSnapshotComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::DownloadEdgeSnapshotRequest& request,
        std::vector<::bosdyn::api::graph_nav::DownloadEdgeSnapshotResponse>&& response,
        const grpc::Status& status, std::promise<DownloadEdgeSnapshotResultType> promise);

};

}  // namespace client

}  // namespace bosdyn
