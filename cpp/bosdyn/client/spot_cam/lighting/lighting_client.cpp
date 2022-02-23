/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/spot_cam/lighting/lighting_client.h"
#include "bosdyn/common/success_condition.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

namespace spot_cam {

const char* LightingClient::s_default_service_name = "spot-cam-lighting";
const char* LightingClient::s_service_type = "bosdyn.api.spot_cam.LightingService";

std::shared_future<GetLEDBrightnessResultType> LightingClient::GetLEDBrightnessAsync(
    const RPCParameters& parameters) {
    std::promise<GetLEDBrightnessResultType> response;
    std::shared_future<GetLEDBrightnessResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::spot_cam::GetLEDBrightnessRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::GetLEDBrightnessRequest,
                          ::bosdyn::api::spot_cam::GetLEDBrightnessResponse,
                          ::bosdyn::api::spot_cam::GetLEDBrightnessResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::LightingService::Stub::AsyncGetLEDBrightness, m_stub.get(),
                      _1, _2, _3),
            std::bind(&LightingClient::OnGetLEDBrightnessComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

GetLEDBrightnessResultType LightingClient::GetLEDBrightness(
    const RPCParameters& parameters) {
    return GetLEDBrightnessAsync(parameters).get();
}

void LightingClient::OnGetLEDBrightnessComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::GetLEDBrightnessRequest& request,
    ::bosdyn::api::spot_cam::GetLEDBrightnessResponse&& response, const grpc::Status& status,
    std::promise<GetLEDBrightnessResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::GetLEDBrightnessResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<SetLEDBrightnessResultType> LightingClient::SetLEDBrightnessAsync(
    const std::map<int32_t, float>& brightnesses,
    const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::SetLEDBrightnessRequest request;
    auto map = request.mutable_brightnesses();
    map->insert(brightnesses.begin(), brightnesses.end());

    return SetLEDBrightnessAsync(request, parameters);
}

SetLEDBrightnessResultType LightingClient::SetLEDBrightness(
    const std::map<int32_t, float>& brightnesses,
    const RPCParameters& parameters) {
    return SetLEDBrightnessAsync(brightnesses, parameters).get();
}

std::shared_future<SetLEDBrightnessResultType> LightingClient::SetLEDBrightnessAsync(
    ::bosdyn::api::spot_cam::SetLEDBrightnessRequest& request,
    const RPCParameters& parameters) {
    std::promise<SetLEDBrightnessResultType> response;
    std::shared_future<SetLEDBrightnessResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::SetLEDBrightnessRequest,
                          ::bosdyn::api::spot_cam::SetLEDBrightnessResponse,
                          ::bosdyn::api::spot_cam::SetLEDBrightnessResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::LightingService::Stub::AsyncSetLEDBrightness, m_stub.get(),
                      _1, _2, _3),
            std::bind(&LightingClient::OnSetLEDBrightnessComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

SetLEDBrightnessResultType LightingClient::SetLEDBrightness(
    ::bosdyn::api::spot_cam::SetLEDBrightnessRequest& request,
    const RPCParameters& parameters) {
    return SetLEDBrightnessAsync(request, parameters).get();
}

void LightingClient::OnSetLEDBrightnessComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::SetLEDBrightnessRequest& request,
    ::bosdyn::api::spot_cam::SetLEDBrightnessResponse&& response, const grpc::Status& status,
    std::promise<SetLEDBrightnessResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::SetLEDBrightnessResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

ServiceClient::QualityOfService LightingClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void LightingClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::spot_cam::LightingService::Stub(channel));
}

}  // namespace spot_cam
}  // namespace client

}  // namespace bosdyn
