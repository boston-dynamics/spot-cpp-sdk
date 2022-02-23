/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/robot_state/robot_state_client.h"

#include <string>

#include "bosdyn/client/error_codes/sdk_error_code.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* RobotStateClient::s_default_service_name = "robot-state";

const char* RobotStateClient::s_service_type = "bosdyn.api.RobotStateService";

// Asynchronous method to get dynamic robot state.
std::shared_future<RobotStateResultType> RobotStateClient::GetRobotStateAsync(
    const RPCParameters& parameters) {
    std::promise<RobotStateResultType> response;
    std::shared_future<RobotStateResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::RobotStateRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::RobotStateRequest, ::bosdyn::api::RobotStateResponse, ::bosdyn::api::RobotStateResponse>(
            request,
            std::bind(&::bosdyn::api::RobotStateService::Stub::AsyncGetRobotState, m_stub.get(), _1, _2, _3),
            std::bind(&RobotStateClient::OnGetRobotStateComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

// Callback for GetRobotState.
void RobotStateClient::OnGetRobotStateComplete(
    MessagePumpCallBase*, const ::bosdyn::api::RobotStateRequest&, ::bosdyn::api::RobotStateResponse&& response,
    const grpc::Status& status, std::promise<RobotStateResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::RobotStateResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

// Synchronous method to get dynamic robot state.
RobotStateResultType RobotStateClient::GetRobotState(
    const RPCParameters& parameters) {
    return this->GetRobotStateAsync(parameters).get();
}

// Asynchronous method to get robot metrics.
std::shared_future<RobotMetricsResultType> RobotStateClient::GetRobotMetricsAsync(
    const RPCParameters& parameters) {
    std::promise<RobotMetricsResultType> response;
    std::shared_future<RobotMetricsResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::RobotMetricsRequest request;
    MessagePumpCallBase* one_time = InitiateAsyncCall<
        ::bosdyn::api::RobotMetricsRequest, ::bosdyn::api::RobotMetricsResponse, ::bosdyn::api::RobotMetricsResponse>(
        request,
        std::bind(&::bosdyn::api::RobotStateService::Stub::AsyncGetRobotMetrics, m_stub.get(), _1, _2, _3),
        std::bind(&RobotStateClient::OnGetRobotMetricsComplete, this, _1, _2, _3, _4, _5),
        std::move(response), parameters);

    return future;
}

// Callback for GetRobotMetrics.
void RobotStateClient::OnGetRobotMetricsComplete(
    MessagePumpCallBase*, const ::bosdyn::api::RobotMetricsRequest&, ::bosdyn::api::RobotMetricsResponse&& response,
    const grpc::Status& status, std::promise<RobotMetricsResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::RobotMetricsResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

// Synchronous method to get the robot metrics.
RobotMetricsResultType RobotStateClient::GetRobotMetrics(
    const RPCParameters& parameters) {
    return this->GetRobotMetricsAsync(parameters).get();
}

// Asynchronous method to get the robot hardware configuration.
std::shared_future<HardwareConfigurationResultType>
RobotStateClient::GetRobotHardwareConfigurationAsync(
    const RPCParameters& parameters) {
    std::promise<HardwareConfigurationResultType> response;
    std::shared_future<HardwareConfigurationResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::RobotHardwareConfigurationRequest request;
    MessagePumpCallBase* one_time = InitiateAsyncCall<::bosdyn::api::RobotHardwareConfigurationRequest,
                                                      ::bosdyn::api::RobotHardwareConfigurationResponse,
                                                      ::bosdyn::api::RobotHardwareConfigurationResponse>(
        request,
        std::bind(&::bosdyn::api::RobotStateService::Stub::AsyncGetRobotHardwareConfiguration, m_stub.get(),
                  _1, _2, _3),
        std::bind(&RobotStateClient::OnGetHardwareConfigurationComplete, this, _1, _2, _3, _4, _5),
        std::move(response), parameters);
    return future;
}

// Callback for GetRobotHardwareConfiguration.
void RobotStateClient::OnGetHardwareConfigurationComplete(
    MessagePumpCallBase*, const ::bosdyn::api::RobotHardwareConfigurationRequest&,
    ::bosdyn::api::RobotHardwareConfigurationResponse&& response, const grpc::Status& status,
    std::promise<HardwareConfigurationResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::RobotHardwareConfigurationResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

// Synchronous method to get the robot hardware configuration.
HardwareConfigurationResultType RobotStateClient::GetRobotHardwareConfiguration(
    const RPCParameters& parameters) {
    return this->GetRobotHardwareConfigurationAsync(parameters).get();
}

// Asynchronous method to get OBJ file for a specific robot link.
std::shared_future<LinkObjectModelResultType> RobotStateClient::GetRobotLinkModelAsync(
    const std::string& link_name, const RPCParameters& parameters) {
    std::promise<LinkObjectModelResultType> response;
    std::shared_future<LinkObjectModelResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::RobotLinkModelRequest request;
    request.set_link_name(link_name);
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::RobotLinkModelRequest, ::bosdyn::api::RobotLinkModelResponse,
                          ::bosdyn::api::RobotLinkModelResponse>(
            request,
            std::bind(&::bosdyn::api::RobotStateService::Stub::AsyncGetRobotLinkModel, m_stub.get(), _1, _2,
                      _3),
            std::bind(&RobotStateClient::OnGetRobotLinkObjectModelComplete, this, _1, _2, _3, _4,
                      _5),
            std::move(response), parameters);

    return future;
}

// Callback for GetRobotLinkModel
void RobotStateClient::OnGetRobotLinkObjectModelComplete(
    MessagePumpCallBase*, const ::bosdyn::api::RobotLinkModelRequest& request,
    ::bosdyn::api::RobotLinkModelResponse&& response, const grpc::Status& status,
    std::promise<LinkObjectModelResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::RobotLinkModelResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

// Synchronous method to get OBJ file for a specific robot link.
LinkObjectModelResultType RobotStateClient::GetRobotLinkModel(
    const std::string& link_name, const RPCParameters& parameters) {
    return this->GetRobotLinkModelAsync(link_name, parameters).get();
}

ServiceClient::QualityOfService RobotStateClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void RobotStateClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::RobotStateService::Stub(channel));
}

}  // namespace client

}  // namespace bosdyn
