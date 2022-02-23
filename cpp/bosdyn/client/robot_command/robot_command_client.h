/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/robot_command.pb.h>
#include <bosdyn/api/robot_command_service.grpc.pb.h>
#include <bosdyn/api/robot_command_service.pb.h>

#include <future>

#include "robot_command_error_codes.h"
#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/common/status.h"
#include "bosdyn/client/lease/lease.h"
#include "bosdyn/client/lease/lease_processors.h"
#include "bosdyn/client/time_sync/time_sync_helpers.h"

namespace bosdyn {

namespace client {

// The return types for each RPC callback that provides the status of the RPC call in addition
// to the data/response message the RPC was returning from the robot.
typedef Result<::bosdyn::api::RobotCommandResponse> RobotCommandResultType;
typedef Result<::bosdyn::api::RobotCommandFeedbackResponse> RobotCommandFeedbackResultType;
typedef Result<::bosdyn::api::ClearBehaviorFaultResponse> ClearBehaviorFaultResultType;

/**
 * The RobotCommand service handles robot locomotion commands and provides feedback on command
 * status. This creates a client which communicates to the RobotCommand service and can:
 *    - Command the robot.
 *    - Request feedback on a robot command.
 *    - Clear behavior faults.
 */
class RobotCommandClient : public ServiceClient {
 public:
    RobotCommandClient() = default;
    ~RobotCommandClient() = default;

    // Asynchronous method to issue a robot command.
    std::shared_future<RobotCommandResultType> RobotCommandAsync(
        ::bosdyn::api::RobotCommandRequest& request,
        const RPCParameters& parameters = RPCParameters());
    std::shared_future<RobotCommandResultType> RobotCommandAsync(
        ::bosdyn::api::RobotCommand& command, Lease* lease = nullptr,
        TimeSyncEndpoint* time_sync_endpoint = nullptr,
        ::bosdyn::common::TimePoint end_time = ::bosdyn::common::TimePoint(::bosdyn::common::Duration(0)),
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to issue a robot command.
    RobotCommandResultType RobotCommand(
        ::bosdyn::api::RobotCommandRequest& request,
        const RPCParameters& parameters = RPCParameters());
    RobotCommandResultType RobotCommand(
        ::bosdyn::api::RobotCommand& command, Lease* lease = nullptr,
        TimeSyncEndpoint* time_sync_endpoint = nullptr,
        ::bosdyn::common::TimePoint end_time = ::bosdyn::common::TimePoint(::bosdyn::common::Duration(0)),
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to request robot command feedback, which indicates the state
    // of a robot command, none of which are considered errors.
    std::shared_future<RobotCommandFeedbackResultType> RobotCommandFeedbackAsync(
        ::bosdyn::api::RobotCommandFeedbackRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to request robot command feedback.
    RobotCommandFeedbackResultType RobotCommandFeedback(
        ::bosdyn::api::RobotCommandFeedbackRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to clear a robot behavior fault.
    std::shared_future<ClearBehaviorFaultResultType> ClearBehaviorFaultAsync(
        ::bosdyn::api::ClearBehaviorFaultRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to clear a robot behavior fault.
    ClearBehaviorFaultResultType ClearBehaviorFault(
        ::bosdyn::api::ClearBehaviorFaultRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    // Sets the QualityOfService enum for the robot state client to be used for network selection
    // optimization.
    QualityOfService GetQualityOfService() const override;

    // Set the communication details for the robot state service, including the stub and processors
    // for requests/responses.
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;

    // Override the update from method to set the lease wallet for the robot command client
    // to be the same as the lease wallet of the robot object.
    void UpdateServiceFrom(
        RequestProcessorChain& request_processor_chain,
        ResponseProcessorChain& response_processor_chain,
        const std::shared_ptr<LeaseWallet>& lease_wallet) override;
    // End of ServiceClient overrides.

    // Get the default service name the RobotCommand service will be registered in the directory
    // with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the lease wallet for the robot command service.
    std::shared_ptr<LeaseWallet> GetLeaseWallet() { return m_lease_wallet; }

    // Add a timesync endpoint into the client. This will be used to convert command end times.
    void AddTimeSyncEndpoint(TimeSyncEndpoint* endpoint) { m_time_sync_endpoint = endpoint; };

    // Get the default service type for the robot command service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous calls to issue a robot command.
    void OnRobotCommandComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::RobotCommandRequest& request,
        ::bosdyn::api::RobotCommandResponse&& response, const grpc::Status& status,
        std::promise<RobotCommandResultType> promise);

    // Callback function registered for the asynchronous calls to request command feedback.
    void OnRobotCommandFeedbackComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::RobotCommandFeedbackRequest& request,
        ::bosdyn::api::RobotCommandFeedbackResponse&& response, const grpc::Status& status,
        std::promise<RobotCommandFeedbackResultType> promise);

    // Callback function registered for the asynchronous calls to clear a behavior fault.
    void OnClearBehaviorFaultComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::ClearBehaviorFaultRequest& request,
        ::bosdyn::api::ClearBehaviorFaultResponse&& response, const grpc::Status& status,
        std::promise<ClearBehaviorFaultResultType> promise);

    std::unique_ptr<::bosdyn::api::RobotCommandService::Stub> m_stub;

    // Default service name for the robot command service.
    static const char* s_default_service_name;

    // Default service type for the robot command service.
    static const char* s_service_type;

    // Lease wallet for commands.
    std::shared_ptr<LeaseWallet> m_lease_wallet;

    // Timesync endpoint to update command endtimes. Note, the TimeSyncEndpoint this pointer is
    // referencing is owned by the Robot object's TimeSyncThread. The pointer will be populated in
    // the robot command client using the AddTimeSyncEndpoint method.
    TimeSyncEndpoint* m_time_sync_endpoint = nullptr;

    // Helper function to update the robot command messages that contain and require a timestamp.
    void MutateEndTime(
        ::bosdyn::api::RobotCommand* robot_command, const google::protobuf::Timestamp& end_time);

    void MutateEndTime(
        ::bosdyn::api::MobilityCommand::Request* mobility_command,
        const google::protobuf::Timestamp& end_time);
};

}  // namespace client

}  // namespace bosdyn
