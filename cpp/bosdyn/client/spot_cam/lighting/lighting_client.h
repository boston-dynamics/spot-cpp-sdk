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
#include "bosdyn/client/spot_cam/lighting/lighting_error_codes.h"

#include <bosdyn/api/spot_cam/service.grpc.pb.h>
#include <bosdyn/api/spot_cam/service.pb.h>
#include <future>

namespace bosdyn {

namespace client {

namespace spot_cam {

typedef Result<::bosdyn::api::spot_cam::GetLEDBrightnessResponse> GetLEDBrightnessResultType;
typedef Result<::bosdyn::api::spot_cam::SetLEDBrightnessResponse> SetLEDBrightnessResultType;

class LightingClient : public ServiceClient {
 public:
    LightingClient() = default;

    ~LightingClient() = default;

    std::shared_future<GetLEDBrightnessResultType> GetLEDBrightnessAsync(
        const RPCParameters& parameters = RPCParameters());

    GetLEDBrightnessResultType GetLEDBrightness(
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<SetLEDBrightnessResultType> SetLEDBrightnessAsync(
        const std::map<int32_t, float>& brightnesses,
        const RPCParameters& parameters = RPCParameters());

    SetLEDBrightnessResultType SetLEDBrightness(
        const std::map<int32_t, float>& brightnesses,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<SetLEDBrightnessResultType> SetLEDBrightnessAsync(
        ::bosdyn::api::spot_cam::SetLEDBrightnessRequest& request,
        const RPCParameters& parameters = RPCParameters());

    SetLEDBrightnessResultType SetLEDBrightness(
        ::bosdyn::api::spot_cam::SetLEDBrightnessRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the spot cam lighting service will be registered in the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the spot cam lighting service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous grpc calls.
    void OnGetLEDBrightnessComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::GetLEDBrightnessRequest& request,
        ::bosdyn::api::spot_cam::GetLEDBrightnessResponse&& response, const grpc::Status& status,
        std::promise<GetLEDBrightnessResultType> promise);
    void OnSetLEDBrightnessComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::SetLEDBrightnessRequest& request,
        ::bosdyn::api::spot_cam::SetLEDBrightnessResponse&& response, const grpc::Status& status,
        std::promise<SetLEDBrightnessResultType> promise);

    std::unique_ptr<::bosdyn::api::spot_cam::LightingService::Stub> m_stub;

    // Default service name for the spotcam lighting service.
    static const char* s_default_service_name;

    // Default service type for the spotcam lighting service.
    static const char* s_service_type;
};

}  // namespace spot_cam
}  // namespace client

}  // namespace bosdyn
