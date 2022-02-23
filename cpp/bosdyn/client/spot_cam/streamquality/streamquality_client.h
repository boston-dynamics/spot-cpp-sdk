/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/client/error_codes/proto_enum_to_stderror_macro.h"

#include <bosdyn/api/spot_cam/service.grpc.pb.h>
#include <bosdyn/api/spot_cam/service.pb.h>
#include <bosdyn/api/spot_cam/streamquality.pb.h>
#include <future>

namespace bosdyn {

namespace client {

namespace spot_cam {

typedef Result<::bosdyn::api::spot_cam::GetStreamParamsResponse> GetStreamParamsResultType;
typedef Result<::bosdyn::api::spot_cam::SetStreamParamsResponse> SetStreamParamsResultType;
typedef Result<::bosdyn::api::spot_cam::EnableCongestionControlResponse> EnableCongestionControlResultType;

class StreamQualityClient : public ServiceClient {
 public:
    StreamQualityClient() = default;

    ~StreamQualityClient() = default;

    std::shared_future<GetStreamParamsResultType> GetStreamParamsAsync(
        const RPCParameters& parameters = RPCParameters());

    GetStreamParamsResultType GetStreamParams(const RPCParameters& parameters = RPCParameters());

    std::shared_future<SetStreamParamsResultType> SetStreamParamsAsync(
        int targetbitrate, int refreshinterval, int idrinterval,
        ::bosdyn::api::spot_cam::StreamParams_AwbModeEnum awb,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<SetStreamParamsResultType> SetStreamParamsAsync(
        ::bosdyn::api::spot_cam::StreamParams params, const RPCParameters& parameters = RPCParameters());

    SetStreamParamsResultType SetStreamParams(int targetbitrate, int refreshinterval,
                                              int idrinterval,
                                              ::bosdyn::api::spot_cam::StreamParams_AwbModeEnum awb,
                                              const RPCParameters& parameters = RPCParameters());

    SetStreamParamsResultType SetStreamParams(::bosdyn::api::spot_cam::StreamParams params,
                                            const RPCParameters& parameters = RPCParameters());

    std::shared_future<SetStreamParamsResultType> SetStreamParamsAsync(
        ::bosdyn::api::spot_cam::SetStreamParamsRequest& request,
        const RPCParameters& parameters = RPCParameters());

    SetStreamParamsResultType SetStreamParams(::bosdyn::api::spot_cam::SetStreamParamsRequest& request,
                                            const RPCParameters& parameters = RPCParameters());

    std::shared_future<EnableCongestionControlResultType> EnableCongestionControlAsync(
        bool enable, const RPCParameters& parameters = RPCParameters());

    EnableCongestionControlResultType EnableCongestionControl(
        bool enable, const RPCParameters& parameters = RPCParameters());

    std::shared_future<EnableCongestionControlResultType> EnableCongestionControlAsync(
        ::bosdyn::api::spot_cam::EnableCongestionControlRequest& request,
        const RPCParameters& parameters = RPCParameters());

    EnableCongestionControlResultType EnableCongestionControl(
        ::bosdyn::api::spot_cam::EnableCongestionControlRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channe) override;
    // End of ServiceClient overrides.

    // Get the default service name the spot cam streamquality service will be registered in the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the spot cam streamquality service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous grpc calls.
    void OnGetStreamParamsComplete(
        MessagePumpCallBase* call,
        const ::bosdyn::api::spot_cam::GetStreamParamsRequest& request,
        ::bosdyn::api::spot_cam::GetStreamParamsResponse&& response,
        const grpc::Status& status,
        std::promise<GetStreamParamsResultType> promise);

    void OnSetStreamParamsComplete(
        MessagePumpCallBase* call,
        const ::bosdyn::api::spot_cam::SetStreamParamsRequest& request,
        ::bosdyn::api::spot_cam::SetStreamParamsResponse&& response,
        const grpc::Status& status,
        std::promise<SetStreamParamsResultType> promise);

    void OnEnableCongestionControlComplete(
        MessagePumpCallBase* call,
        const ::bosdyn::api::spot_cam::EnableCongestionControlRequest& request,
        ::bosdyn::api::spot_cam::EnableCongestionControlResponse&& response,
        const grpc::Status& status,
        std::promise<EnableCongestionControlResultType> promise);

    std::unique_ptr<::bosdyn::api::spot_cam::StreamQualityService::Stub> m_stub;

    // Default service name for the spotcam streamquality service.
    static const char* s_default_service_name;

    // Default service type for the spotcam streamquality service.
    static const char* s_service_type;
};

}  // namespace spot_cam
}  // namespace client

}  // namespace bosdyn
