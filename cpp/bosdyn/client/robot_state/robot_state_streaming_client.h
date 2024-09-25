/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/robot_state.pb.h>
#include <bosdyn/api/robot_state_service.grpc.pb.h>
#include <bosdyn/api/robot_state_service.pb.h>

#include "bosdyn/client/error_codes/joint_control_stream_error_code.h"
#include "bosdyn/client/service_client/service_client.h"

namespace bosdyn {

namespace client {

// The return types for each RPC callback that provides the status of the RPC call in addition
// to the data/response message the RPC was returning from the robot.
typedef Result<::bosdyn::api::RobotStateStreamResponse> RobotStateStreamResultType;

class RobotStateStreamingClient : public ServiceClient {
 public:
    // Constructor for the RobotStateStreaming client.
    RobotStateStreamingClient() = default;

    // Destructor for the RobotStateStreaming client.
    ~RobotStateStreamingClient() = default;

    // Synchronous method to get dynamic robot state.
    RobotStateStreamResultType GetRobotStateStream();

    // Sets the QualityOfService enum for the robot state client to be used for network selection
    // optimization.
    QualityOfService GetQualityOfService() const override;

    // Set the communication details for the robot state service, including the stub and processors
    // for requests/responses.
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;

    // Get the default service name the RobotStateStream service will be registered in the directory
    // with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the RobotStateStream service that will be registered in the
    // directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // The stub for the robot state client to communicate it's rpc with (and pass to the robot).
    std::unique_ptr<::bosdyn::api::RobotStateStreamingService::StubInterface> m_stub;

    // Default service name for the RobotStateStreaming service.
    static const char* s_default_service_name;

    // Default service type for the RobotStateStreaming service.
    static const char* s_service_type;

    // The ClientContext
    ::grpc::ClientContext m_context;

    std::unique_ptr<::grpc::ClientReaderInterface<::bosdyn::api::RobotStateStreamResponse>>
        m_response_reader;
};

}  // namespace client

}  // namespace bosdyn
