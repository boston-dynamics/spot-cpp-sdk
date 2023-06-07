/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "inverse_kinematics_client.h"
#include "bosdyn/client/error_codes/rpc_error_code.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* InverseKinematicsClient::s_default_service_name = "inverse-kinematics";

const char* InverseKinematicsClient::s_service_type = "bosdyn.api.InverseKinematicsService";

std::shared_future<InverseKinematicsResultType> InverseKinematicsClient::InverseKinematicsAsync(
    ::bosdyn::api::spot::InverseKinematicsRequest& request, const RPCParameters& parameters) {
    std::promise<InverseKinematicsResultType> response;
    std::shared_future<InverseKinematicsResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot::InverseKinematicsRequest,
                          ::bosdyn::api::spot::InverseKinematicsResponse,
                          ::bosdyn::api::spot::InverseKinematicsResponse>(
            request,
            std::bind(&::bosdyn::api::spot::InverseKinematicsService::Stub::AsyncInverseKinematics,
                      m_stub.get(), _1, _2, _3),
            std::bind(&InverseKinematicsClient::OnInverseKinematicsComplete, this, _1, _2, _3, _4,
                      _5),
            std::move(response), parameters);

    return future;
}

void InverseKinematicsClient::OnInverseKinematicsComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot::InverseKinematicsRequest& request,
    ::bosdyn::api::spot::InverseKinematicsResponse&& response, const grpc::Status& status,
    std::promise<InverseKinematicsResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseAndGetFinalStatus<::bosdyn::api::spot::InverseKinematicsResponse>(
            status, response, response.status());
    promise.set_value({ret_status, std::move(response)});
}

InverseKinematicsResultType InverseKinematicsClient::InverseKinematics(
    ::bosdyn::api::spot::InverseKinematicsRequest& request, const RPCParameters& parameters) {
    return InverseKinematicsAsync(request, parameters).get();
}

ServiceClient::QualityOfService InverseKinematicsClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void InverseKinematicsClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::spot::InverseKinematicsService::Stub(channel));
}

}  // namespace client

}  // namespace bosdyn
