/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/spot_cam/ptz/ptz_client.h"
#include "bosdyn/common/success_condition.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

namespace spot_cam {

const char* PtzClient::s_default_service_name = "spot-cam-ptz";
const char* PtzClient::s_service_type = "bosdyn.api.spot_cam.PtzService";

std::shared_future<ListPtzResultType> PtzClient::ListPtzAsync(
    const RPCParameters& parameters) {
    std::promise<ListPtzResultType> response;
    std::shared_future<ListPtzResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::spot_cam::ListPtzRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::ListPtzRequest, ::bosdyn::api::spot_cam::ListPtzResponse,
                          ::bosdyn::api::spot_cam::ListPtzResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::PtzService::Stub::AsyncListPtz, m_stub.get(), _1, _2, _3),
            std::bind(&PtzClient::OnListPtzComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

ListPtzResultType PtzClient::ListPtz(
    const RPCParameters& parameters) {
    return ListPtzAsync(parameters).get();
}

void PtzClient::OnListPtzComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::ListPtzRequest& request,
    ::bosdyn::api::spot_cam::ListPtzResponse&& response, const grpc::Status& status,
    std::promise<ListPtzResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::ListPtzResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<GetPtzPositionResultType> PtzClient::GetPtzPositionAsync(
    const std::string& ptz_name, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::GetPtzPositionRequest request;
    request.mutable_ptz()->set_name(ptz_name);

    return GetPtzPositionAsync(request, parameters);
}

GetPtzPositionResultType PtzClient::GetPtzPosition(
    const std::string& ptz_name, const RPCParameters& parameters) {
    return GetPtzPositionAsync(ptz_name, parameters).get();
}

std::shared_future<GetPtzPositionResultType> PtzClient::GetPtzPositionAsync(
    ::bosdyn::api::spot_cam::GetPtzPositionRequest& request,
    const RPCParameters& parameters) {
    std::promise<GetPtzPositionResultType> response;
    std::shared_future<GetPtzPositionResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::GetPtzPositionRequest,
                          ::bosdyn::api::spot_cam::GetPtzPositionResponse,
                          ::bosdyn::api::spot_cam::GetPtzPositionResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::PtzService::Stub::AsyncGetPtzPosition, m_stub.get(), _1, _2,
                      _3),
            std::bind(&PtzClient::OnGetPtzPositionComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

GetPtzPositionResultType PtzClient::GetPtzPosition(
    ::bosdyn::api::spot_cam::GetPtzPositionRequest& request,
    const RPCParameters& parameters) {
    return GetPtzPositionAsync(request, parameters).get();
}

void PtzClient::OnGetPtzPositionComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::GetPtzPositionRequest& request,
    ::bosdyn::api::spot_cam::GetPtzPositionResponse&& response, const grpc::Status& status,
    std::promise<GetPtzPositionResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::GetPtzPositionResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<GetPtzVelocityResultType> PtzClient::GetPtzVelocityAsync(
    const std::string& ptz_name, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::GetPtzVelocityRequest request;
    request.mutable_ptz()->set_name(ptz_name);
    return GetPtzVelocityAsync(request, parameters);
}

GetPtzVelocityResultType PtzClient::GetPtzVelocity(
    const std::string& ptz_name, const RPCParameters& parameters) {
    return GetPtzVelocityAsync(ptz_name, parameters).get();
}

std::shared_future<GetPtzVelocityResultType> PtzClient::GetPtzVelocityAsync(
    ::bosdyn::api::spot_cam::GetPtzVelocityRequest& request,
    const RPCParameters& parameters) {
    std::promise<GetPtzVelocityResultType> response;
    std::shared_future<GetPtzVelocityResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::GetPtzVelocityRequest,
                          ::bosdyn::api::spot_cam::GetPtzVelocityResponse,
                          ::bosdyn::api::spot_cam::GetPtzVelocityResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::PtzService::Stub::AsyncGetPtzVelocity, m_stub.get(), _1, _2,
                      _3),
            std::bind(&PtzClient::OnGetPtzVelocityComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

GetPtzVelocityResultType PtzClient::GetPtzVelocity(
    ::bosdyn::api::spot_cam::GetPtzVelocityRequest& request,
    const RPCParameters& parameters) {
    return GetPtzVelocityAsync(request, parameters).get();
}

void PtzClient::OnGetPtzVelocityComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::GetPtzVelocityRequest& request,
    ::bosdyn::api::spot_cam::GetPtzVelocityResponse&& response, const grpc::Status& status,
    std::promise<GetPtzVelocityResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::GetPtzVelocityResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<SetPtzPositionResultType> PtzClient::SetPtzPositionAsync(
    const std::string& ptz_name, float pan, float tilt, float zoom,
    const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::SetPtzPositionRequest request;
    request.mutable_position()->mutable_ptz()->set_name(ptz_name);
    request.mutable_position()->mutable_pan()->set_value(pan);
    request.mutable_position()->mutable_tilt()->set_value(tilt);
    request.mutable_position()->mutable_zoom()->set_value(zoom);

    return SetPtzPositionAsync(request, parameters);
}

SetPtzPositionResultType PtzClient::SetPtzPosition(
    const std::string& ptz_name, float pan, float tilt, float zoom,
    const RPCParameters& parameters) {
    return SetPtzPositionAsync(ptz_name, pan, tilt, zoom, parameters).get();
}

std::shared_future<SetPtzPositionResultType> PtzClient::SetPtzPositionAsync(
    ::bosdyn::api::spot_cam::SetPtzPositionRequest& request,
    const RPCParameters& parameters) {
    std::promise<SetPtzPositionResultType> response;
    std::shared_future<SetPtzPositionResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::SetPtzPositionRequest,
                          ::bosdyn::api::spot_cam::SetPtzPositionResponse,
                          ::bosdyn::api::spot_cam::SetPtzPositionResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::PtzService::Stub::AsyncSetPtzPosition, m_stub.get(), _1, _2,
                      _3),
            std::bind(&PtzClient::OnSetPtzPositionComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

SetPtzPositionResultType PtzClient::SetPtzPosition(
    ::bosdyn::api::spot_cam::SetPtzPositionRequest& request,
    const RPCParameters& parameters) {
    return SetPtzPositionAsync(request, parameters).get();
}

void PtzClient::OnSetPtzPositionComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::SetPtzPositionRequest& request,
    ::bosdyn::api::spot_cam::SetPtzPositionResponse&& response, const grpc::Status& status,
    std::promise<SetPtzPositionResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::SetPtzPositionResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<SetPtzVelocityResultType> PtzClient::SetPtzVelocityAsync(
    const std::string& ptz_name, float pan, float tilt, float zoom,
    const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::SetPtzVelocityRequest request;
    request.mutable_velocity()->mutable_ptz()->set_name(ptz_name);
    request.mutable_velocity()->mutable_pan()->set_value(pan);
    request.mutable_velocity()->mutable_tilt()->set_value(tilt);
    request.mutable_velocity()->mutable_zoom()->set_value(zoom);

    return SetPtzVelocityAsync(request, parameters);
}

SetPtzVelocityResultType PtzClient::SetPtzVelocity(
    const std::string& ptz_name, float pan, float tilt, float zoom,
    const RPCParameters& parameters) {
    return SetPtzVelocityAsync(ptz_name, pan, tilt, zoom, parameters).get();
}

std::shared_future<SetPtzVelocityResultType> PtzClient::SetPtzVelocityAsync(
    ::bosdyn::api::spot_cam::SetPtzVelocityRequest& request,
    const RPCParameters& parameters) {
    std::promise<SetPtzVelocityResultType> response;
    std::shared_future<SetPtzVelocityResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::SetPtzVelocityRequest,
                          ::bosdyn::api::spot_cam::SetPtzVelocityResponse,
                          ::bosdyn::api::spot_cam::SetPtzVelocityResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::PtzService::Stub::AsyncSetPtzVelocity, m_stub.get(), _1, _2,
                      _3),
            std::bind(&PtzClient::OnSetPtzVelocityComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

SetPtzVelocityResultType PtzClient::SetPtzVelocity(
    ::bosdyn::api::spot_cam::SetPtzVelocityRequest& request,
    const RPCParameters& parameters) {
    return SetPtzVelocityAsync(request, parameters).get();
}

void PtzClient::OnSetPtzVelocityComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::SetPtzVelocityRequest& request,
    ::bosdyn::api::spot_cam::SetPtzVelocityResponse&& response, const grpc::Status& status,
    std::promise<SetPtzVelocityResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::SetPtzVelocityResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<InitializeLensResultType> PtzClient::InitializeLensAsync(
    const RPCParameters& parameters) {
    std::promise<InitializeLensResultType> response;
    std::shared_future<InitializeLensResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::spot_cam::InitializeLensRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::InitializeLensRequest, ::bosdyn::api::spot_cam::InitializeLensResponse,
                          ::bosdyn::api::spot_cam::InitializeLensResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::PtzService::Stub::AsyncInitializeLens, m_stub.get(), _1, _2, _3),
            std::bind(&PtzClient::OnInitializeLensComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

InitializeLensResultType PtzClient::InitializeLens(const RPCParameters& parameters) {
    return InitializeLensAsync(parameters).get();
}

void PtzClient::OnInitializeLensComplete(MessagePumpCallBase* call,
                                         const ::bosdyn::api::spot_cam::InitializeLensRequest& request,
                                         ::bosdyn::api::spot_cam::InitializeLensResponse&& response,
                                         const grpc::Status& status,
                                         std::promise<InitializeLensResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::InitializeLensResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

ServiceClient::QualityOfService PtzClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void PtzClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::spot_cam::PtzService::Stub(channel));
}

}  // namespace spot_cam
}  // namespace client

}  // namespace bosdyn
