/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/robot_id/robot_id_client.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* RobotIdClient::s_default_service_name = "robot-id";

const char* RobotIdClient::s_default_service_authority = "id.spot.robot";

const char* RobotIdClient::s_service_type = "bosdyn.api.RobotIdService";

std::shared_future<RobotIdResultType> RobotIdClient::GetAsync(
    const RPCParameters& parameters) {
    std::promise<RobotIdResultType> response;
    std::shared_future<RobotIdResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::RobotIdRequest request;
    MessagePumpCallBase* one_time = InitiateAsyncCall<::bosdyn::api::RobotIdRequest, ::bosdyn::api::RobotIdResponse,
                                                      ::bosdyn::api::RobotIdResponse>(
        request, std::bind(&::bosdyn::api::RobotIdService::Stub::AsyncGetRobotId, m_stub.get(), _1, _2, _3),
        std::bind(&RobotIdClient::OnGetRobotIdComplete, this, _1, _2, _3, _4, _5),
        std::move(response), parameters);

    return future;
}

RobotIdResultType RobotIdClient::Get(const RPCParameters& parameters) {
    return GetAsync(parameters).get();
}

// Set the outstanding response, which will update any futures. Reset it.
void RobotIdClient::OnGetRobotIdComplete(MessagePumpCallBase* call,
                                         const ::bosdyn::api::RobotIdRequest& request,
                                         ::bosdyn::api::RobotIdResponse&& response, const grpc::Status& status,
                                         std::promise<RobotIdResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::RobotIdResponse>(
        status, response, SDKErrorCode::Success);
    if (!ret_status) {
        promise.set_value({ret_status, std::move(response)});
        return;
    }

    promise.set_value({ret_status, std::move(response)});
}

ServiceClient::QualityOfService RobotIdClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void RobotIdClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::RobotIdService::Stub(channel));
}

}  // namespace client

}  // namespace bosdyn
