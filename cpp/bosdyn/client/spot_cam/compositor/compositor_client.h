/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <map>

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/client/spot_cam/compositor/compositor_error_codes.h"

#include <bosdyn/api/spot_cam/service.grpc.pb.h>
#include <bosdyn/api/spot_cam/service.pb.h>
#include <future>

namespace bosdyn {

namespace client {

namespace spot_cam {

typedef Result<::bosdyn::api::spot_cam::SetScreenResponse> SetScreenResultType;
typedef Result<::bosdyn::api::spot_cam::GetScreenResponse> GetScreenResultType;
typedef Result<::bosdyn::api::spot_cam::ListScreensResponse> ListScreensResultType;
typedef Result<::bosdyn::api::spot_cam::GetVisibleCamerasResponse> GetVisibleCamerasResultType;
typedef Result<::bosdyn::api::spot_cam::GetIrColormapResponse> GetIrColormapResultType;
typedef Result<::bosdyn::api::spot_cam::SetIrColormapResponse> SetIrColormapResultType;
typedef Result<::bosdyn::api::spot_cam::SetIrMeterOverlayResponse> SetIrMeterOverlayResultType;

class CompositorClient : public ServiceClient {
 public:
    CompositorClient() = default;

    ~CompositorClient() = default;

    std::shared_future<SetScreenResultType> SetScreenAsync(
        const std::string& name, const RPCParameters& parameters = RPCParameters());

    SetScreenResultType SetScreen(const std::string& name,
                                  const RPCParameters& parameters = RPCParameters());

    std::shared_future<SetScreenResultType> SetScreenAsync(
        ::bosdyn::api::spot_cam::SetScreenRequest& request,
        const RPCParameters& parameters = RPCParameters());

    SetScreenResultType SetScreen(::bosdyn::api::spot_cam::SetScreenRequest& request,
                                  const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetScreenResultType> GetScreenAsync(
        const RPCParameters& parameters = RPCParameters());

    GetScreenResultType GetScreen(const RPCParameters& parameters = RPCParameters());

    std::shared_future<ListScreensResultType> ListScreensAsync(
        const RPCParameters& parameters = RPCParameters());

    ListScreensResultType ListScreens(const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetVisibleCamerasResultType> GetVisibleCamerasAsync(
        const RPCParameters& parameters = RPCParameters());
    GetVisibleCamerasResultType GetVisibleCameras(
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetIrColormapResultType> GetIrColormapAsync(
        const RPCParameters& parameters = RPCParameters());
    GetIrColormapResultType GetIrColormap(const RPCParameters& parameters = RPCParameters());

    std::shared_future<SetIrColormapResultType> SetIrColormapAsync(
        const ::bosdyn::api::spot_cam::IrColorMap::ColorMap& colormap, float min_temp, float max_temp,
        bool auto_scale, const RPCParameters& parameters = RPCParameters());
    SetIrColormapResultType SetIrColormap(const ::bosdyn::api::spot_cam::IrColorMap::ColorMap& colormap,
                                          float min_temp, float max_temp, bool auto_scale,
                                          const RPCParameters& parameters = RPCParameters());
    std::shared_future<SetIrColormapResultType> SetIrColormapAsync(
        ::bosdyn::api::spot_cam::SetIrColormapRequest& request,
        const RPCParameters& parameters = RPCParameters());
    SetIrColormapResultType SetIrColormap(::bosdyn::api::spot_cam::SetIrColormapRequest& request,
                                          const RPCParameters& parameters = RPCParameters());

    std::shared_future<SetIrMeterOverlayResultType> SetIrMeterOverlayAsync(
        float x, float y, bool enable, const RPCParameters& parameters = RPCParameters());
    SetIrMeterOverlayResultType SetIrMeterOverlay(
        float x, float y, bool enable, const RPCParameters& parameters = RPCParameters());
    std::shared_future<SetIrMeterOverlayResultType> SetIrMeterOverlayAsync(
        ::bosdyn::api::spot_cam::SetIrMeterOverlayRequest& request,
        const RPCParameters& parameters = RPCParameters());
    SetIrMeterOverlayResultType SetIrMeterOverlay(
        ::bosdyn::api::spot_cam::SetIrMeterOverlayRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the spot cam compositor service will be registered in the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the spot cam compositor service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous grpc calls.
    void OnSetScreenComplete(MessagePumpCallBase* call,
                             const ::bosdyn::api::spot_cam::SetScreenRequest& request,
                             ::bosdyn::api::spot_cam::SetScreenResponse&& response, const grpc::Status& status,
                             std::promise<SetScreenResultType> promise);
    void OnGetScreenComplete(MessagePumpCallBase* call,
                             const ::bosdyn::api::spot_cam::GetScreenRequest& request,
                             ::bosdyn::api::spot_cam::GetScreenResponse&& response, const grpc::Status& status,
                             std::promise<GetScreenResultType> promise);
    void OnListScreensComplete(MessagePumpCallBase* call,
                               const ::bosdyn::api::spot_cam::ListScreensRequest& request,
                               ::bosdyn::api::spot_cam::ListScreensResponse&& response,
                               const grpc::Status& status, std::promise<ListScreensResultType> promise);
    void OnGetVisibleCamerasComplete(MessagePumpCallBase* call,
                                     const ::bosdyn::api::spot_cam::GetVisibleCamerasRequest& request,
                                     ::bosdyn::api::spot_cam::GetVisibleCamerasResponse&& response,
                                     const grpc::Status& status,
                                     std::promise<GetVisibleCamerasResultType> promise);
    void OnGetIrColormapComplete(MessagePumpCallBase* call,
                                 const ::bosdyn::api::spot_cam::GetIrColormapRequest& request,
                                 ::bosdyn::api::spot_cam::GetIrColormapResponse&& response,
                                 const grpc::Status& status,
                                 std::promise<GetIrColormapResultType> promise);
    void OnSetIrColormapComplete(MessagePumpCallBase* call,
                                 const ::bosdyn::api::spot_cam::SetIrColormapRequest& request,
                                 ::bosdyn::api::spot_cam::SetIrColormapResponse&& response,
                                 const grpc::Status& status,
                                 std::promise<SetIrColormapResultType> promise);
    void OnSetIrMeterOverlayComplete(MessagePumpCallBase* call,
                                     const ::bosdyn::api::spot_cam::SetIrMeterOverlayRequest& request,
                                     ::bosdyn::api::spot_cam::SetIrMeterOverlayResponse&& response,
                                     const grpc::Status& status,
                                     std::promise<SetIrMeterOverlayResultType> promise);

    std::unique_ptr<::bosdyn::api::spot_cam::CompositorService::Stub> m_stub;

    // Default service name for the spot cam compositor service.
    static const char* s_default_service_name;

    // Default service type for the spotcam compositor service.
    static const char* s_service_type;
};

}  // namespace spot_cam
}  // namespace client

}  // namespace bosdyn
