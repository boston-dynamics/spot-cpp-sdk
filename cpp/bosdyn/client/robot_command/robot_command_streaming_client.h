/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/robot_command.pb.h>
#include <bosdyn/api/robot_command_service.grpc.pb.h>
#include <bosdyn/api/robot_command_service.pb.h>

#include "bosdyn/client/error_codes/joint_control_stream_error_code.h"
#include "bosdyn/client/service_client/service_client.h"

namespace bosdyn {

namespace client {

// The return types for each RPC callback that provides the status of the RPC call in addition
// to the data/response message the RPC was returning from the robot.
typedef Result<::bosdyn::api::JointControlStreamResponse> JointControlStreamResultType;

class RobotCommandStreamingClient : public ServiceClient {
 public:
    RobotCommandStreamingClient() = default;
    ~RobotCommandStreamingClient() = default;

    JointControlStreamResultType JointControlStream(
        const ::bosdyn::api::JointControlStreamRequest& request);

    // Start of ServiceClient overrides.
    // Sets the QualityOfService enum for the robot command streaming client to be used for network
    // selection optimization.
    QualityOfService GetQualityOfService() const override;

    // Set the communication details for the robot command streaming service, including the stub and
    // processors for requests/responses.
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;

    // Get the default service name the RobotCommandStreaming service will be registered in the
    // directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the robot command streaming service that will be registered
    // in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    std::unique_ptr<::bosdyn::api::RobotCommandStreamingService::StubInterface> m_stub;

    // Default service name for the robot command service.
    static const char* s_default_service_name;

    // Default service type for the robot command service.
    static const char* s_service_type;

    ::bosdyn::api::JointControlStreamResponse m_response;

    ::grpc::ClientContext m_context;

    std::unique_ptr<::grpc::ClientWriterInterface<::bosdyn::api::JointControlStreamRequest>>
        m_request_writer;
};

}  // namespace client

}  // namespace bosdyn
