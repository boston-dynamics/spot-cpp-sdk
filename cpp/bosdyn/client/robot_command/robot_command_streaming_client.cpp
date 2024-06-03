/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "robot_command_streaming_client.h"

namespace bosdyn {

namespace client {

const char* RobotCommandStreamingClient::s_default_service_name = "robot-command-streaming";

const char* RobotCommandStreamingClient::s_service_type = "bosdyn.api.RobotCommandStreamingService";

JointControlStreamResultType RobotCommandStreamingClient::JointControlStream(
    const ::bosdyn::api::JointControlStreamRequest& request) {
    if (!m_request_writer) {
        m_request_writer = m_stub->JointControlStream(&m_context, &m_response);
        if (!m_request_writer) {
            return {::bosdyn::common::Status(JointControlStreamErrorCode::RequestWriterFailed),
                    std::move(m_response)};
        }
    }

    if (!m_request_writer->Write(request)) {
        return {::bosdyn::common::Status(JointControlStreamErrorCode::StreamingFailed),
                std::move(m_response)};
    }

    return {::bosdyn::common::Status(JointControlStreamErrorCode::Success)};
}

ServiceClient::QualityOfService RobotCommandStreamingClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void RobotCommandStreamingClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::RobotCommandStreamingService::Stub(channel));
}

}  // namespace client

}  // namespace bosdyn
