/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/robot_state/robot_state_streaming_client.h"

namespace bosdyn {

namespace client {

const char* RobotStateStreamingClient::s_default_service_name = "robot-state-streaming";

const char* RobotStateStreamingClient::s_service_type = "bosdyn.api.RobotStateStreamingService";

RobotStateStreamResultType RobotStateStreamingClient::GetRobotStateStream() {
    ::bosdyn::api::RobotStateStreamRequest request;
    ::bosdyn::api::RobotStateStreamResponse response;
    if (!m_response_reader) {
        m_response_reader = m_stub->GetRobotStateStream(&m_context, request);
        if (!m_response_reader) {
            return {::bosdyn::common::Status(JointControlStreamErrorCode::ResponseReaderFailed),
                    std::move(response)};
        }
    }

    if (!m_response_reader->Read(&response)) {
        return {::bosdyn::common::Status(JointControlStreamErrorCode::StreamingFailed),
                std::move(response)};
    }
    return {::bosdyn::common::Status(JointControlStreamErrorCode::Success), std::move(response)};
}

ServiceClient::QualityOfService RobotStateStreamingClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void RobotStateStreamingClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::RobotStateStreamingService::Stub(channel));
}

}  // namespace client
}  // namespace bosdyn