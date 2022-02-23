/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <future>

#include <bosdyn/api/docking/docking_service.grpc.pb.h>
#include <bosdyn/api/docking/docking_service.pb.h>

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/common/time.h"
#include "bosdyn/client/docking/docking_error_codes.h"
#include "bosdyn/common/status.h"
#include "bosdyn/client/time_sync/time_sync_helpers.h"

namespace bosdyn {

namespace client {

typedef Result<::bosdyn::api::docking::DockingCommandResponse> DockingCommandResultType;
typedef Result<::bosdyn::api::docking::DockingCommandFeedbackResponse> DockingCommandFeedbackResultType;
typedef Result<::bosdyn::api::docking::GetDockingConfigResponse> GetDockingConfigResultType;
typedef Result<::bosdyn::api::docking::GetDockingStateResponse> GetDockingStateResultType;

// DockingClient is the GRPC client for the Docking service defined in docking_service.proto.
class DockingClient : public ServiceClient {
 public:
    DockingClient() = default;

    ~DockingClient() = default;

    // Asynchronous method to request the dock ID ranges.
    std::shared_future<GetDockingConfigResultType> GetDockingConfigAsync(
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to request the dock ID ranges.
    GetDockingConfigResultType GetDockingConfig(
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to request the docking service state.
    std::shared_future<GetDockingStateResultType> GetDockingStateAsync(
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to request the docking service state.
    GetDockingStateResultType GetDockingState(
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to start docking the robot.
    std::shared_future<DockingCommandResultType> DockingCommandAsync(
        ::bosdyn::api::docking::DockingCommandRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to start doing the robot.
    DockingCommandResultType DockingCommand(
        ::bosdyn::api::docking::DockingCommandRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to request docking command feedback. The status field in the return
    // object does not incorporate the value of the status field in the protobuf response object
    // because the status of a feedback method is not considered an error.
    std::shared_future<DockingCommandFeedbackResultType> DockingCommandFeedbackAsync(
        ::bosdyn::api::docking::DockingCommandFeedbackRequest& request,
        const RPCParameters& parameters = RPCParameters());

    DockingCommandFeedbackResultType DockingCommandFeedback(
        ::bosdyn::api::docking::DockingCommandFeedbackRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<DockingCommandFeedbackResultType> DockingCommandFeedbackAsync(
        unsigned int id, const RPCParameters& parameters = RPCParameters());

    // Synchronous method to request docking command feedback. The status field in the return object
    // does not incorporate the value of the status field in the protobuf response object because
    // the status of a feedback method is not considered an error.
    DockingCommandFeedbackResultType DockingCommandFeedback(
        unsigned int id, const RPCParameters& parameters = RPCParameters());

    // Building a DockingCommandRequest from a dock ID and end time (in local time). This will
    // create the request with end_time using robot's synced time.
    Result<::bosdyn::api::docking::DockingCommandRequest> DockingCommandBuilder(
        unsigned int dock_id, ::bosdyn::common::TimePoint local_end_time,
        TimeSyncEndpoint* time_sync_endpoint = nullptr);

    // Add a timesync endpoint into the client. This will be used to convert command end times.
    void AddTimeSyncEndpoint(TimeSyncEndpoint* endpoint) { m_time_sync_endpoint = endpoint; };

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;

    // Override the update from method to set the lease wallet for the robot command client
    // to be the same as the lease wallet of the robot object.
    void UpdateServiceFrom(RequestProcessorChain& request_processor_chain,
                           ResponseProcessorChain& response_processor_chain,
                           const std::shared_ptr<LeaseWallet>& lease_wallet) override;
    // End of ServiceClient overrides.

    // Get the default service name the Docking service will be registered in the directory
    // with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the Docking service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback that will return the GetDockingConfigResponse message after GetDockingConfig rpc
    // returns to the client.
    void OnGetDockingConfigComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::docking::GetDockingConfigRequest& request,
        ::bosdyn::api::docking::GetDockingConfigResponse&& response, const grpc::Status& status,
        std::promise<GetDockingConfigResultType> promise);

    // Callback that will return the GetDockingStateResponse message after GetDockingState rpc
    // returns to the client.
    void OnGetDockingStateComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::docking::GetDockingStateRequest& request,
        ::bosdyn::api::docking::GetDockingStateResponse&& response, const grpc::Status& status,
        std::promise<GetDockingStateResultType> promise);

    // Callback that will return the DockingCommandResponse message after DockingCommand rpc returns
    // to the client.
    void OnDockingCommandComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::docking::DockingCommandRequest& request,
        ::bosdyn::api::docking::DockingCommandResponse&& response, const grpc::Status& status,
        std::promise<DockingCommandResultType> promise);

    // Callback that will return the DockingCommandResponse message after DockingCommand rpc returns
    // to the client.
    void OnDockingCommandFeedbackComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::docking::DockingCommandFeedbackRequest& request,
        ::bosdyn::api::docking::DockingCommandFeedbackResponse&& response, const grpc::Status& status,
        std::promise<DockingCommandFeedbackResultType> promise);

    std::unique_ptr<::bosdyn::api::docking::DockingService::Stub> m_stub;

    // Lease wallet for commands.
    std::shared_ptr<LeaseWallet> m_lease_wallet;

    // Default service name for the Docking service.
    static const char* s_default_service_name;

    // Default service type for the Docking service.
    static const char* s_service_type;

    // Timesync endpoint to update command end times. Note, the TimeSyncEndpoint this pointer is
    // referencing is owned by the Robot object's TimeSyncThread. The pointer will be populated in
    // the robot command client using the AddTimeSyncEndpoint method.
    TimeSyncEndpoint* m_time_sync_endpoint = nullptr;
};

}  // namespace client

}  // namespace bosdyn
