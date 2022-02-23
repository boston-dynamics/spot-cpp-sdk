/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/robot_state.pb.h>
#include <bosdyn/api/robot_state_service.grpc.pb.h>
#include <bosdyn/api/robot_state_service.pb.h>

#include <future>
#include <memory>
#include <string>

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/common/status.h"

namespace bosdyn {

namespace client {

// The return types for each RPC callback that provides the status of the RPC call in addition
// to the data/response message the RPC was returning from the robot.
typedef Result<::bosdyn::api::RobotStateResponse> RobotStateResultType;
typedef Result<::bosdyn::api::RobotMetricsResponse> RobotMetricsResultType;
typedef Result<::bosdyn::api::RobotHardwareConfigurationResponse> HardwareConfigurationResultType;
typedef Result<::bosdyn::api::RobotLinkModelResponse> LinkObjectModelResultType;

/**
 * The RobotState service tracks all information about the measured and computed states of the
 * robot at the current time. This creates a client which communicates to the RobotState service
 * and can request information about:
 *    - The current physical state of the robot.
 *    - The different metrics and parameters associated with the robot.
 *    - The current hardware configuration (links and joints) for the robot.
 *    - Specific link object models.
 */
class RobotStateClient : public ServiceClient {
 public:
    // Constructor for the RobotState client, which can be used to make RPC requests for the
    // current robot state, robot metrics, hardware configuration, and specific link object models.
    RobotStateClient() { m_RPC_parameters.logging_control = LogRequestMode::kEnabled; }

    // Destructor for the RobotState client.
    ~RobotStateClient() = default;

    // Asynchronous method to get dynamic robot state, which includes the kinematic state, behavior
    // and system faults, estop state, battery/power states, and comms state.
    std::shared_future<RobotStateResultType> GetRobotStateAsync(
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to get dynamic robot state.
    RobotStateResultType GetRobotState(
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to get the robot metrics, which includes different robot parameters and
    // the associated descriptions, units, and values.
    std::shared_future<RobotMetricsResultType> GetRobotMetricsAsync(
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to get the robot metrics.
    RobotMetricsResultType GetRobotMetrics(
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to get the robot hardware configuration, which includes the robot skeleton
    // containing all link and joint object models and urdf.
    std::shared_future<HardwareConfigurationResultType> GetRobotHardwareConfigurationAsync(
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to get the robot hardware configuration.
    HardwareConfigurationResultType GetRobotHardwareConfiguration(
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to get OBJ file for a specific robot link. Typically this RPC is called
    // after getting the full hardware configuration such that a specific link name can be used.
    std::shared_future<LinkObjectModelResultType> GetRobotLinkModelAsync(
        const std::string& link_name,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to get OBJ file for a specific robot link.
    LinkObjectModelResultType GetRobotLinkModel(
        const std::string& link_name,
        const RPCParameters& parameters = RPCParameters());

    // Sets the QualityOfService enum for the robot state client to be used for network selection
    // optimization.
    QualityOfService GetQualityOfService() const override;

    // Set the communication details for the robot state service, including the stub and processors
    // for requests/responses.
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;

    // Get the default service name the RobotState service will be registered in the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the RobotState service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // The stub for the robot state client to communicate it's rpc with (and pass to the robot).
    std::unique_ptr<::bosdyn::api::RobotStateService::Stub> m_stub;

    // Default service name for the RobotState service.
    static const char* s_default_service_name;

    // Default service type for the RobotState service.
    static const char* s_service_type;

    // Callback that will return the current RobotState message after GetRobotState rpc returns to
    // the client.
    void OnGetRobotStateComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::RobotStateRequest& request,
        ::bosdyn::api::RobotStateResponse&& response, const grpc::Status& status,
        std::promise<RobotStateResultType> promise);

    // Callback that will return the current RobotMetrics message after GetRobotMetrics rpc returns
    // to the client.
    void OnGetRobotMetricsComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::RobotMetricsRequest& request,
        ::bosdyn::api::RobotMetricsResponse&& response, const grpc::Status& status,
        std::promise<RobotMetricsResultType> promise);

    // Callback that will return the current HardwareConfiguration message after
    // GetRobotHardwareConfiguration rpc returns to the client.
    void OnGetHardwareConfigurationComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::RobotHardwareConfigurationRequest& request,
        ::bosdyn::api::RobotHardwareConfigurationResponse&& response, const grpc::Status& status,
        std::promise<HardwareConfigurationResultType> promise);

    // Callback that will return the current link object model message for the specific link
    // requested after the GetRobotLinkObjectModel rpc returns to the client.
    void OnGetRobotLinkObjectModelComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::RobotLinkModelRequest& request,
        ::bosdyn::api::RobotLinkModelResponse&& response, const grpc::Status& status,
        std::promise<LinkObjectModelResultType> promise);
};

}  // namespace client

}  // namespace bosdyn
