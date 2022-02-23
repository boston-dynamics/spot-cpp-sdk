/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/spot_cam/streamquality/streamquality_client.h"
#include "bosdyn/common/success_condition.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

namespace spot_cam {

const char* StreamQualityClient::s_default_service_name = "spot-cam-streamquality";
const char* StreamQualityClient::s_service_type = "bosdyn.api.spot_cam.StreamQualityService";

std::shared_future<GetStreamParamsResultType> StreamQualityClient::GetStreamParamsAsync(
    const RPCParameters& parameters) {
    std::promise<GetStreamParamsResultType> response;
    std::shared_future<GetStreamParamsResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::spot_cam::GetStreamParamsRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::GetStreamParamsRequest,
                          ::bosdyn::api::spot_cam::GetStreamParamsResponse,
                          ::bosdyn::api::spot_cam::GetStreamParamsResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::StreamQualityService::Stub::AsyncGetStreamParams, m_stub.get(), _1, _2,
                      _3),
            std::bind(&StreamQualityClient::OnGetStreamParamsComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

GetStreamParamsResultType StreamQualityClient::GetStreamParams(const RPCParameters& parameters) {
    return GetStreamParamsAsync(parameters).get();
}

void StreamQualityClient::OnGetStreamParamsComplete(MessagePumpCallBase* call,
                                           const ::bosdyn::api::spot_cam::GetStreamParamsRequest& request,
                                           ::bosdyn::api::spot_cam::GetStreamParamsResponse&& response,
                                           const grpc::Status& status,
                                           std::promise<GetStreamParamsResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::GetStreamParamsResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<SetStreamParamsResultType> StreamQualityClient::SetStreamParamsAsync(
    int targetbitrate, int refreshinterval, int idrinterval, ::bosdyn::api::spot_cam::StreamParams_AwbModeEnum awb, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::SetStreamParamsRequest request;
    request.mutable_params()->mutable_targetbitrate()->set_value(targetbitrate);
    request.mutable_params()->mutable_refreshinterval()->set_value(refreshinterval);
    request.mutable_params()->mutable_idrinterval()->set_value(idrinterval);
    request.mutable_params()->mutable_awb()->set_awb(awb);

    return SetStreamParamsAsync(request, parameters);
}

std::shared_future<SetStreamParamsResultType> StreamQualityClient::SetStreamParamsAsync(
    ::bosdyn::api::spot_cam::StreamParams params, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::SetStreamParamsRequest request;
    request.mutable_params()->CopyFrom(params);

    return SetStreamParamsAsync(request, parameters);
}

SetStreamParamsResultType StreamQualityClient::SetStreamParams(
    int targetbitrate, int refreshinterval, int idrinterval,
    ::bosdyn::api::spot_cam::StreamParams_AwbModeEnum awb, const RPCParameters& parameters) {
    return SetStreamParamsAsync(targetbitrate, refreshinterval, idrinterval, awb, parameters).get();
}

SetStreamParamsResultType StreamQualityClient::SetStreamParams(::bosdyn::api::spot_cam::StreamParams params,
                                                     const RPCParameters& parameters) {
    return SetStreamParamsAsync(params, parameters).get();
}

std::shared_future<SetStreamParamsResultType> StreamQualityClient::SetStreamParamsAsync(
    ::bosdyn::api::spot_cam::SetStreamParamsRequest& request, const RPCParameters& parameters) {
    std::promise<SetStreamParamsResultType> response;
    std::shared_future<SetStreamParamsResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::SetStreamParamsRequest,
                          ::bosdyn::api::spot_cam::SetStreamParamsResponse,
                          ::bosdyn::api::spot_cam::SetStreamParamsResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::StreamQualityService::Stub::AsyncSetStreamParams, m_stub.get(), _1, _2,
                      _3),
            std::bind(&StreamQualityClient::OnSetStreamParamsComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

SetStreamParamsResultType StreamQualityClient::SetStreamParams(
    ::bosdyn::api::spot_cam::SetStreamParamsRequest& request,
    const RPCParameters& parameters) {
    return SetStreamParamsAsync(request, parameters).get();
}

void StreamQualityClient::OnSetStreamParamsComplete(MessagePumpCallBase* call,
                                           const ::bosdyn::api::spot_cam::SetStreamParamsRequest& request,
                                           ::bosdyn::api::spot_cam::SetStreamParamsResponse&& response,
                                           const grpc::Status& status,
                                           std::promise<SetStreamParamsResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::SetStreamParamsResponse>(
        status, response, SDKErrorCode::Success);
    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<EnableCongestionControlResultType> StreamQualityClient::EnableCongestionControlAsync(
    bool enable, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::EnableCongestionControlRequest request;
    request.set_enable_congestion_control(enable);
    return EnableCongestionControlAsync(request, parameters);
}

EnableCongestionControlResultType StreamQualityClient::EnableCongestionControl(
    bool enable, const RPCParameters& parameters) {
    return EnableCongestionControlAsync(enable, parameters).get();
}

std::shared_future<EnableCongestionControlResultType> StreamQualityClient::EnableCongestionControlAsync(
    ::bosdyn::api::spot_cam::EnableCongestionControlRequest& request, const RPCParameters& parameters) {
    std::promise<EnableCongestionControlResultType> response;
    std::shared_future<EnableCongestionControlResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::EnableCongestionControlRequest,
                          ::bosdyn::api::spot_cam::EnableCongestionControlResponse,
                          ::bosdyn::api::spot_cam::EnableCongestionControlResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::StreamQualityService::Stub::AsyncEnableCongestionControl, m_stub.get(), _1, _2,
                      _3),
            std::bind(&StreamQualityClient::OnEnableCongestionControlComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

EnableCongestionControlResultType StreamQualityClient::EnableCongestionControl(
    ::bosdyn::api::spot_cam::EnableCongestionControlRequest& request,
    const RPCParameters& parameters) {
    return EnableCongestionControlAsync(request, parameters).get();
}

void StreamQualityClient::OnEnableCongestionControlComplete(
    MessagePumpCallBase* call,
    const ::bosdyn::api::spot_cam::EnableCongestionControlRequest& request,
    ::bosdyn::api::spot_cam::EnableCongestionControlResponse&& response,
    const grpc::Status& status,
    std::promise<EnableCongestionControlResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::EnableCongestionControlResponse>(
        status, response, SDKErrorCode::Success);
    promise.set_value({ret_status, std::move(response)});
}

ServiceClient::QualityOfService StreamQualityClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void StreamQualityClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::spot_cam::StreamQualityService::Stub(channel));
}

}  // namespace spot_cam
}  // namespace client

}  // namespace bosdyn
