/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/graph_nav/recording_service.grpc.pb.h>
#include <bosdyn/api/graph_nav/recording_service.pb.h>

#include <future>

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/common/status.h"
#include "bosdyn/client/graph_nav/recording_error_codes.h"

namespace bosdyn {

namespace client {

// Return type for the StartRecording method.
typedef Result<::bosdyn::api::graph_nav::StartRecordingResponse> StartRecordingResultType;

// Return type for the StopRecording method.
typedef Result<::bosdyn::api::graph_nav::StopRecordingResponse> StopRecordingResultType;

// Return type for the CreateWaypoint method.
typedef Result<::bosdyn::api::graph_nav::CreateWaypointResponse> CreateWaypointResultType;

// Return type for the SetRecordingEnvironment method.
typedef Result<::bosdyn::api::graph_nav::SetRecordingEnvironmentResponse> SetRecordingEnvironmentResultType;

// Return type for the CreateEdge method.
typedef Result<::bosdyn::api::graph_nav::CreateEdgeResponse> CreateEdgeResultType;

// Return type for the GetRecordStatus method.
typedef Result<::bosdyn::api::graph_nav::GetRecordStatusResponse> GetRecordStatusResultType;

class GraphNavRecordingClient : public ServiceClient {
 public:
    // Constructor for the GraphNavRecording client, which can be used to make RPC requests for
    // GraphNavRecording commands.
    GraphNavRecordingClient() = default;

    // Destructor for the GraphNavRecording client.
    ~GraphNavRecordingClient() = default;

    // Asynchronous method to execute a StartRecording request.
    std::shared_future<StartRecordingResultType> StartRecordingAsync(
        ::bosdyn::api::graph_nav::StartRecordingRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to execute a StartRecording request.
    StartRecordingResultType StartRecording(::bosdyn::api::graph_nav::StartRecordingRequest& request,
                                            const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to execute a StopRecording request.
    std::shared_future<StopRecordingResultType> StopRecordingAsync(
        ::bosdyn::api::graph_nav::StopRecordingRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to execute a StopRecording request.
    StopRecordingResultType StopRecording(::bosdyn::api::graph_nav::StopRecordingRequest& request,
                                          const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to execute a CreateWaypoint request.
    std::shared_future<CreateWaypointResultType> CreateWaypointAsync(
        ::bosdyn::api::graph_nav::CreateWaypointRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to execute a CreateWaypoint request.
    CreateWaypointResultType CreateWaypoint(::bosdyn::api::graph_nav::CreateWaypointRequest& request,
                                            const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to execute a SetRecordingEnvironment request.
    std::shared_future<SetRecordingEnvironmentResultType> SetRecordingEnvironmentAsync(
        ::bosdyn::api::graph_nav::SetRecordingEnvironmentRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to execute a SetRecordingEnvironment request.
    SetRecordingEnvironmentResultType SetRecordingEnvironment(
        ::bosdyn::api::graph_nav::SetRecordingEnvironmentRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to execute a CreateEdge request.
    std::shared_future<CreateEdgeResultType> CreateEdgeAsync(
        ::bosdyn::api::graph_nav::CreateEdgeRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to execute a CreateEdge request.
    CreateEdgeResultType CreateEdge(::bosdyn::api::graph_nav::CreateEdgeRequest& request,
                                    const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to execute a GetRecordStatus request.
    std::shared_future<GetRecordStatusResultType> GetRecordStatusAsync(
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to execute a GetRecordStatus request.
    GetRecordStatusResultType GetRecordStatus(const RPCParameters& parameters = RPCParameters());

    // Sets the QualityOfService enum for the GraphNavRecording client to be used for network
    // selection optimization.
    QualityOfService GetQualityOfService() const override;

    // Set the communication details for the GraphNavRecording service, including the stub and
    // processors for requests/responses.
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;

    // Override the update from method to set the lease wallet for the GraphNavRecording client
    // to be the same as the lease wallet of the robot object.
    void UpdateServiceFrom(RequestProcessorChain& request_processor_chain,
                           ResponseProcessorChain& response_processor_chain,
                           const std::shared_ptr<LeaseWallet>& lease_wallet) override;

    // Get the default service name the GraphNavRecording service will be registered in the
    // directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the image service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // The stub for the GraphNavRecording client to communicate it's rpc with (and pass to the
    // robot).
    std::unique_ptr<::bosdyn::api::graph_nav::GraphNavRecordingService::Stub> m_stub;

    // Default service name for the GraphNavRecording service.
    static const char* s_default_service_name;

    // Default service type for the recording service.
    static const char* s_service_type;

    // Lease wallet for commands.
    std::shared_ptr<LeaseWallet> m_lease_wallet;

    // Callback that will return the StartRecordingResponse message after StartRecording rpc
    // returns to the client.
    void OnStartRecordingComplete(MessagePumpCallBase* call,
                                  const ::bosdyn::api::graph_nav::StartRecordingRequest& request,
                                  ::bosdyn::api::graph_nav::StartRecordingResponse&& response,
                                  const grpc::Status& status,
                                  std::promise<StartRecordingResultType> promise);

    // Callback that will return the StopRecordingResponse message after StopRecording rpc
    // returns to the client.
    void OnStopRecordingComplete(MessagePumpCallBase* call,
                                 const ::bosdyn::api::graph_nav::StopRecordingRequest& request,
                                 ::bosdyn::api::graph_nav::StopRecordingResponse&& response,
                                 const grpc::Status& status,
                                 std::promise<StopRecordingResultType> promise);

    // Callback that will return the CreateWaypointResponse message after CreateWaypoint rpc
    // returns to the client.
    void OnCreateWaypointComplete(MessagePumpCallBase* call,
                                  const ::bosdyn::api::graph_nav::CreateWaypointRequest& request,
                                  ::bosdyn::api::graph_nav::CreateWaypointResponse&& response,
                                  const grpc::Status& status,
                                  std::promise<CreateWaypointResultType> promise);

    // Callback that will return the SetRecordingEnvironmentResponse message after
    // SetRecordingEnvironment rpc returns to the client.
    void OnSetRecordingEnvironmentComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::graph_nav::SetRecordingEnvironmentRequest& request,
        ::bosdyn::api::graph_nav::SetRecordingEnvironmentResponse&& response, const grpc::Status& status,
        std::promise<SetRecordingEnvironmentResultType> promise);

    // Callback that will return the CreateEdgeResponse message after CreateEdge
    // rpc returns to the client.
    void OnCreateEdgeComplete(MessagePumpCallBase* call,
                              const ::bosdyn::api::graph_nav::CreateEdgeRequest& request,
                              ::bosdyn::api::graph_nav::CreateEdgeResponse&& response,
                              const grpc::Status& status,
                              std::promise<CreateEdgeResultType> promise);

    // Callback that will return the GetRecordStatusResponse message after GetRecordStatus rpc
    // returns to the client.
    void OnGetRecordStatusComplete(MessagePumpCallBase* call,
                                   const ::bosdyn::api::graph_nav::GetRecordStatusRequest& request,
                                   ::bosdyn::api::graph_nav::GetRecordStatusResponse&& response,
                                   const grpc::Status& status,
                                   std::promise<GetRecordStatusResultType> promise);
};

}  // namespace client

}  // namespace bosdyn
