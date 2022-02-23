/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/client/spot_cam/ptz/ptz_error_codes.h"

#include <bosdyn/api/spot_cam/service.grpc.pb.h>
#include <bosdyn/api/spot_cam/service.pb.h>
#include <future>

namespace bosdyn {

namespace client {

namespace spot_cam {

typedef Result<::bosdyn::api::spot_cam::ListPtzResponse> ListPtzResultType;
typedef Result<::bosdyn::api::spot_cam::GetPtzPositionResponse> GetPtzPositionResultType;
typedef Result<::bosdyn::api::spot_cam::GetPtzVelocityResponse> GetPtzVelocityResultType;
typedef Result<::bosdyn::api::spot_cam::SetPtzPositionResponse> SetPtzPositionResultType;
typedef Result<::bosdyn::api::spot_cam::SetPtzVelocityResponse> SetPtzVelocityResultType;
typedef Result<::bosdyn::api::spot_cam::InitializeLensResponse> InitializeLensResultType;


class PtzClient : public ServiceClient {
 public:
    PtzClient() {
    }

    ~PtzClient() = default;

    std::shared_future<ListPtzResultType> ListPtzAsync(
        const RPCParameters& parameters = RPCParameters());

    ListPtzResultType ListPtz(
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetPtzPositionResultType> GetPtzPositionAsync(
        const std::string& ptz_name,
        const RPCParameters& parameters = RPCParameters());

    GetPtzPositionResultType GetPtzPosition(
        const std::string& ptz_name,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetPtzPositionResultType> GetPtzPositionAsync(
        ::bosdyn::api::spot_cam::GetPtzPositionRequest& request,
        const RPCParameters& parameters = RPCParameters());

    GetPtzPositionResultType GetPtzPosition(
        ::bosdyn::api::spot_cam::GetPtzPositionRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetPtzVelocityResultType> GetPtzVelocityAsync(
        const std::string& ptz_name,
        const RPCParameters& parameters = RPCParameters());

    GetPtzVelocityResultType GetPtzVelocity(
        const std::string& ptz_name,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetPtzVelocityResultType> GetPtzVelocityAsync(
        ::bosdyn::api::spot_cam::GetPtzVelocityRequest& request,
        const RPCParameters& parameters = RPCParameters());

    GetPtzVelocityResultType GetPtzVelocity(
        ::bosdyn::api::spot_cam::GetPtzVelocityRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<SetPtzPositionResultType> SetPtzPositionAsync(
        const std::string& ptz_name, float pan, float tilt, float zoom,
        const RPCParameters& parameters = RPCParameters());

    SetPtzPositionResultType SetPtzPosition(
        const std::string& ptz_name, float pan, float tilt, float zoom,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<SetPtzPositionResultType> SetPtzPositionAsync(
        ::bosdyn::api::spot_cam::SetPtzPositionRequest& request,
        const RPCParameters& parameters = RPCParameters());

    SetPtzPositionResultType SetPtzPosition(
        ::bosdyn::api::spot_cam::SetPtzPositionRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<SetPtzVelocityResultType> SetPtzVelocityAsync(
        const std::string& ptz_name, float pan, float tilt, float zoom,
        const RPCParameters& parameters = RPCParameters());

    SetPtzVelocityResultType SetPtzVelocity(
        const std::string& ptz_name, float pan, float tilt, float zoom,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<SetPtzVelocityResultType> SetPtzVelocityAsync(
        ::bosdyn::api::spot_cam::SetPtzVelocityRequest& request,
        const RPCParameters& parameters = RPCParameters());

    SetPtzVelocityResultType SetPtzVelocity(
        ::bosdyn::api::spot_cam::SetPtzVelocityRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<InitializeLensResultType> InitializeLensAsync(
        const RPCParameters& parameters = RPCParameters());

    InitializeLensResultType InitializeLens(
        const RPCParameters& parameters = RPCParameters());


    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the spot cam ptz service will be registered in the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the spot cam ptz service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous grpc calls.
    void OnListPtzComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::ListPtzRequest& request,
        ::bosdyn::api::spot_cam::ListPtzResponse&& response, const grpc::Status& status,
        std::promise<ListPtzResultType> promise);
    void OnGetPtzPositionComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::GetPtzPositionRequest& request,
        ::bosdyn::api::spot_cam::GetPtzPositionResponse&& response, const grpc::Status& status,
        std::promise<GetPtzPositionResultType> promise);
    void OnGetPtzVelocityComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::GetPtzVelocityRequest& request,
        ::bosdyn::api::spot_cam::GetPtzVelocityResponse&& response, const grpc::Status& status,
        std::promise<GetPtzVelocityResultType> promise);
    void OnSetPtzPositionComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::SetPtzPositionRequest& request,
        ::bosdyn::api::spot_cam::SetPtzPositionResponse&& response, const grpc::Status& status,
        std::promise<SetPtzPositionResultType> promise);
    void OnSetPtzVelocityComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::SetPtzVelocityRequest& request,
        ::bosdyn::api::spot_cam::SetPtzVelocityResponse&& response, const grpc::Status& status,
        std::promise<SetPtzVelocityResultType> promise);
    void OnInitializeLensComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::InitializeLensRequest& request,
        ::bosdyn::api::spot_cam::InitializeLensResponse&& response, const grpc::Status& status,
        std::promise<InitializeLensResultType> promise);

    std::unique_ptr<::bosdyn::api::spot_cam::PtzService::Stub> m_stub;

    // Default service name for the spotcam ptz service.
    static const char* s_default_service_name;

    // Default service type for the spotcam ptz service.
    static const char* s_service_type;

};

}  // namespace spot_cam
}  // namespace client

}  // namespace bosdyn
