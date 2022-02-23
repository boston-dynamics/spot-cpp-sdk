/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/spot_cam/compositor/compositor_client.h"
#include "bosdyn/common/success_condition.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

namespace spot_cam {

const char* CompositorClient::s_default_service_name = "spot-cam-compositor";

const char* CompositorClient::s_service_type = "bosdyn.api.spot_cam.CompositorService";

std::shared_future<SetScreenResultType> CompositorClient::SetScreenAsync(
    const std::string& name, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::SetScreenRequest request;
    request.set_name(name);

    return SetScreenAsync(request, parameters);
}

SetScreenResultType CompositorClient::SetScreen(const std::string& name,
                                                const RPCParameters& parameters) {
    return SetScreenAsync(name, parameters).get();
}

std::shared_future<SetScreenResultType> CompositorClient::SetScreenAsync(
    ::bosdyn::api::spot_cam::SetScreenRequest& request, const RPCParameters& parameters) {
    std::promise<SetScreenResultType> response;
    std::shared_future<SetScreenResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::SetScreenRequest, ::bosdyn::api::spot_cam::SetScreenResponse,
                          ::bosdyn::api::spot_cam::SetScreenResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::CompositorService::Stub::AsyncSetScreen, m_stub.get(), _1, _2,
                      _3),
            std::bind(&CompositorClient::OnSetScreenComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

SetScreenResultType CompositorClient::SetScreen(::bosdyn::api::spot_cam::SetScreenRequest& request,
                                                const RPCParameters& parameters) {
    return SetScreenAsync(request, parameters).get();
}

void CompositorClient::OnSetScreenComplete(MessagePumpCallBase* call,
                                           const ::bosdyn::api::spot_cam::SetScreenRequest& request,
                                           ::bosdyn::api::spot_cam::SetScreenResponse&& response,
                                           const grpc::Status& status,
                                           std::promise<SetScreenResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::SetScreenResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<GetIrColormapResultType> CompositorClient::GetIrColormapAsync(
    const RPCParameters& parameters) {
    std::promise<GetIrColormapResultType> response;
    std::shared_future<GetIrColormapResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::spot_cam::GetIrColormapRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::GetIrColormapRequest, ::bosdyn::api::spot_cam::GetIrColormapResponse,
                          ::bosdyn::api::spot_cam::GetIrColormapResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::CompositorService::Stub::AsyncGetIrColormap, m_stub.get(), _1,
                      _2, _3),
            std::bind(&CompositorClient::OnGetIrColormapComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

GetIrColormapResultType CompositorClient::GetIrColormap(const RPCParameters& parameters) {
    return GetIrColormapAsync(parameters).get();
}

std::shared_future<SetIrColormapResultType> CompositorClient::SetIrColormapAsync(
    const ::bosdyn::api::spot_cam::IrColorMap::ColorMap& colormap, float min_temp, float max_temp,
    bool auto_scale, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::IrColorMap::ScalingPair scale_pair;
    scale_pair.set_min(min_temp);
    scale_pair.set_max(max_temp);
    google::protobuf::BoolValue bool_val;
    bool_val.set_value(auto_scale);
    ::bosdyn::api::spot_cam::IrColorMap map;
    map.set_colormap(colormap);
    map.mutable_scale()->MergeFrom(scale_pair);
    map.mutable_auto_scale()->MergeFrom(bool_val);
    ::bosdyn::api::spot_cam::SetIrColormapRequest request;
    request.mutable_map()->MergeFrom(map);

    return SetIrColormapAsync(request, parameters);
}
SetIrColormapResultType CompositorClient::SetIrColormap(
    const ::bosdyn::api::spot_cam::IrColorMap::ColorMap& colormap, float min_temp, float max_temp,
    bool auto_scale, const RPCParameters& parameters) {
    return SetIrColormapAsync(colormap, min_temp, max_temp, auto_scale, parameters).get();
}
std::shared_future<SetIrColormapResultType> CompositorClient::SetIrColormapAsync(
    ::bosdyn::api::spot_cam::SetIrColormapRequest& request, const RPCParameters& parameters) {
    std::promise<SetIrColormapResultType> response;
    std::shared_future<SetIrColormapResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::SetIrColormapRequest, ::bosdyn::api::spot_cam::SetIrColormapResponse,
                          ::bosdyn::api::spot_cam::SetIrColormapResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::CompositorService::Stub::AsyncSetIrColormap, m_stub.get(), _1,
                      _2, _3),
            std::bind(&CompositorClient::OnSetIrColormapComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}
SetIrColormapResultType CompositorClient::SetIrColormap(
    ::bosdyn::api::spot_cam::SetIrColormapRequest& request, const RPCParameters& parameters) {
    return SetIrColormapAsync(request, parameters).get();
}

std::shared_future<SetIrMeterOverlayResultType> CompositorClient::SetIrMeterOverlayAsync(
    float x, float y, bool enable, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::SetIrMeterOverlayRequest request;
    ::bosdyn::api::spot_cam::IrMeterOverlay::NormalizedCoordinates coords;
    coords.set_x(x);
    coords.set_y(y);
    ::bosdyn::api::spot_cam::IrMeterOverlay overlay;
    overlay.mutable_coords()->MergeFrom(coords);
    overlay.set_enable(enable);
    request.mutable_overlay()->MergeFrom(overlay);

    return SetIrMeterOverlayAsync(request, parameters);
}
SetIrMeterOverlayResultType CompositorClient::SetIrMeterOverlay(float x, float y, bool enable,
                                                                const RPCParameters& parameters) {
    return SetIrMeterOverlayAsync(x, y, enable, parameters).get();
}
std::shared_future<SetIrMeterOverlayResultType> CompositorClient::SetIrMeterOverlayAsync(
    ::bosdyn::api::spot_cam::SetIrMeterOverlayRequest& request, const RPCParameters& parameters) {
    std::promise<SetIrMeterOverlayResultType> response;
    std::shared_future<SetIrMeterOverlayResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::SetIrMeterOverlayRequest,
                          ::bosdyn::api::spot_cam::SetIrMeterOverlayResponse,
                          ::bosdyn::api::spot_cam::SetIrMeterOverlayResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::CompositorService::Stub::AsyncSetIrMeterOverlay, m_stub.get(),
                      _1, _2, _3),
            std::bind(&CompositorClient::OnSetIrMeterOverlayComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}
SetIrMeterOverlayResultType CompositorClient::SetIrMeterOverlay(
    ::bosdyn::api::spot_cam::SetIrMeterOverlayRequest& request, const RPCParameters& parameters) {
    return SetIrMeterOverlayAsync(request, parameters).get();
}

void CompositorClient::OnGetIrColormapComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::GetIrColormapRequest& request,
    ::bosdyn::api::spot_cam::GetIrColormapResponse&& response, const grpc::Status& status,
    std::promise<GetIrColormapResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::GetIrColormapResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

void CompositorClient::OnSetIrColormapComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::SetIrColormapRequest& request,
    ::bosdyn::api::spot_cam::SetIrColormapResponse&& response, const grpc::Status& status,
    std::promise<SetIrColormapResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::SetIrColormapResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}
void CompositorClient::OnSetIrMeterOverlayComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::SetIrMeterOverlayRequest& request,
    ::bosdyn::api::spot_cam::SetIrMeterOverlayResponse&& response, const grpc::Status& status,
    std::promise<SetIrMeterOverlayResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::SetIrMeterOverlayResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<GetScreenResultType> CompositorClient::GetScreenAsync(
    const RPCParameters& parameters) {
    std::promise<GetScreenResultType> response;
    std::shared_future<GetScreenResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::spot_cam::GetScreenRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::GetScreenRequest, ::bosdyn::api::spot_cam::GetScreenResponse,
                          ::bosdyn::api::spot_cam::GetScreenResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::CompositorService::Stub::AsyncGetScreen, m_stub.get(), _1, _2,
                      _3),
            std::bind(&CompositorClient::OnGetScreenComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

GetScreenResultType CompositorClient::GetScreen(const RPCParameters& parameters) {
    return GetScreenAsync(parameters).get();
}

void CompositorClient::OnGetScreenComplete(MessagePumpCallBase* call,
                                           const ::bosdyn::api::spot_cam::GetScreenRequest& request,
                                           ::bosdyn::api::spot_cam::GetScreenResponse&& response,
                                           const grpc::Status& status,
                                           std::promise<GetScreenResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::GetScreenResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<ListScreensResultType> CompositorClient::ListScreensAsync(
    const RPCParameters& parameters) {
    std::promise<ListScreensResultType> response;
    std::shared_future<ListScreensResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::spot_cam::ListScreensRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::ListScreensRequest, ::bosdyn::api::spot_cam::ListScreensResponse,
                          ::bosdyn::api::spot_cam::ListScreensResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::CompositorService::Stub::AsyncListScreens, m_stub.get(), _1,
                      _2, _3),
            std::bind(&CompositorClient::OnListScreensComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

ListScreensResultType CompositorClient::ListScreens(const RPCParameters& parameters) {
    return ListScreensAsync(parameters).get();
}

void CompositorClient::OnListScreensComplete(MessagePumpCallBase* call,
                                             const ::bosdyn::api::spot_cam::ListScreensRequest& request,
                                             ::bosdyn::api::spot_cam::ListScreensResponse&& response,
                                             const grpc::Status& status,
                                             std::promise<ListScreensResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::ListScreensResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<GetVisibleCamerasResultType> CompositorClient::GetVisibleCamerasAsync(
    const RPCParameters& parameters) {
    std::promise<GetVisibleCamerasResultType> response;
    std::shared_future<GetVisibleCamerasResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::spot_cam::GetVisibleCamerasRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::GetVisibleCamerasRequest,
                          ::bosdyn::api::spot_cam::GetVisibleCamerasResponse,
                          ::bosdyn::api::spot_cam::GetVisibleCamerasResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::CompositorService::Stub::AsyncGetVisibleCameras, m_stub.get(),
                      _1, _2, _3),
            std::bind(&CompositorClient::OnGetVisibleCamerasComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

GetVisibleCamerasResultType CompositorClient::GetVisibleCameras(const RPCParameters& parameters) {
    return GetVisibleCamerasAsync(parameters).get();
}

void CompositorClient::OnGetVisibleCamerasComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::GetVisibleCamerasRequest& request,
    ::bosdyn::api::spot_cam::GetVisibleCamerasResponse&& response, const grpc::Status& status,
    std::promise<GetVisibleCamerasResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::GetVisibleCamerasResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

ServiceClient::QualityOfService CompositorClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void CompositorClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::spot_cam::CompositorService::Stub(channel));
}

}  // namespace spot_cam
}  // namespace client

}  // namespace bosdyn
