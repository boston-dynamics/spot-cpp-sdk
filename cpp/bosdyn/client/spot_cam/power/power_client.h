/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/client/spot_cam/power/power_error_codes.h"

#include <bosdyn/api/spot_cam/service.grpc.pb.h>
#include <bosdyn/api/spot_cam/service.pb.h>
#include <future>

namespace bosdyn {

namespace client {

namespace spot_cam {

typedef Result<::bosdyn::api::spot_cam::GetPowerStatusResponse> GetPowerStatusResultType;
typedef Result<::bosdyn::api::spot_cam::SetPowerStatusResponse> SetPowerStatusResultType;
typedef Result<::bosdyn::api::spot_cam::CyclePowerResponse> CyclePowerResultType;

class PowerClient : public ServiceClient {
 public:
    PowerClient() = default;

    ~PowerClient() = default;

    std::shared_future<GetPowerStatusResultType> GetPowerStatusAsync(
        const RPCParameters& parameters = RPCParameters());

    GetPowerStatusResultType GetPowerStatus(const RPCParameters& parameters = RPCParameters());

    std::shared_future<SetPowerStatusResultType> SetPowerStatusAsync(
        bool ptz, bool aux1, bool aux2, bool external_mic,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<SetPowerStatusResultType> SetPowerStatusAsync(
        bool ptz, bool aux1, bool aux2, const RPCParameters& parameters = RPCParameters());

    std::shared_future<SetPowerStatusResultType> SetPowerStatusAsync(
        bool ptz, bool aux1, const RPCParameters& parameters = RPCParameters());

    std::shared_future<SetPowerStatusResultType> SetPowerStatusAsync(
        bool ptz, const RPCParameters& parameters = RPCParameters());

    SetPowerStatusResultType SetPowerStatus(bool ptz, bool aux1, bool aux2, bool external_mic,
                                            const RPCParameters& parameters = RPCParameters());

    SetPowerStatusResultType SetPowerStatus(bool ptz, bool aux1, bool aux2,
                                            const RPCParameters& parameters = RPCParameters());

    SetPowerStatusResultType SetPowerStatus(bool ptz, bool aux1,
                                            const RPCParameters& parameters = RPCParameters());

    SetPowerStatusResultType SetPowerStatus(bool ptz,
                                            const RPCParameters& parameters = RPCParameters());

    std::shared_future<SetPowerStatusResultType> SetPowerStatusAsync(
        ::bosdyn::api::spot_cam::SetPowerStatusRequest& request,
        const RPCParameters& parameters = RPCParameters());

    SetPowerStatusResultType SetPowerStatus(::bosdyn::api::spot_cam::SetPowerStatusRequest& request,
                                            const RPCParameters& parameters = RPCParameters());

    std::shared_future<CyclePowerResultType> CyclePowerAsync(
        bool ptz, bool aux1, bool aux2, bool external_mic,
        const RPCParameters& parameters = RPCParameters());
    std::shared_future<CyclePowerResultType> CyclePowerAsync(
        bool ptz, bool aux1, bool aux2, const RPCParameters& parameters = RPCParameters());
    std::shared_future<CyclePowerResultType> CyclePowerAsync(
        bool ptz, bool aux1, const RPCParameters& parameters = RPCParameters());
    std::shared_future<CyclePowerResultType> CyclePowerAsync(
        bool ptz, const RPCParameters& parameters = RPCParameters());
    CyclePowerResultType CyclePower(bool ptz, bool aux1, bool aux2, bool external_mic,
                                    const RPCParameters& parameters = RPCParameters());
    CyclePowerResultType CyclePower(bool ptz, bool aux1, bool aux2,
                                    const RPCParameters& parameters = RPCParameters());
    CyclePowerResultType CyclePower(bool ptz, bool aux1,
                                    const RPCParameters& parameters = RPCParameters());
    CyclePowerResultType CyclePower(bool ptz, const RPCParameters& parameters = RPCParameters());
    std::shared_future<CyclePowerResultType> CyclePowerAsync(
        ::bosdyn::api::spot_cam::CyclePowerRequest& request,
        const RPCParameters& parameters = RPCParameters());
    CyclePowerResultType CyclePower(::bosdyn::api::spot_cam::CyclePowerRequest& request,
                                    const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the spotcam power service will be registered in the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the spot cam power service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous grpc calls.
    void OnGetPowerStatusComplete(MessagePumpCallBase* call,
                                  const ::bosdyn::api::spot_cam::GetPowerStatusRequest& request,
                                  ::bosdyn::api::spot_cam::GetPowerStatusResponse&& response,
                                  const grpc::Status& status,
                                  std::promise<GetPowerStatusResultType> promise);
    void OnSetPowerStatusComplete(MessagePumpCallBase* call,
                                  const ::bosdyn::api::spot_cam::SetPowerStatusRequest& request,
                                  ::bosdyn::api::spot_cam::SetPowerStatusResponse&& response,
                                  const grpc::Status& status,
                                  std::promise<SetPowerStatusResultType> promise);
    void OnCyclePowerComplete(MessagePumpCallBase* call,
                              const ::bosdyn::api::spot_cam::CyclePowerRequest& request,
                              ::bosdyn::api::spot_cam::CyclePowerResponse&& response,
                              const grpc::Status& status, std::promise<CyclePowerResultType> promise);

    std::unique_ptr<::bosdyn::api::spot_cam::PowerService::Stub> m_stub;

    // Default service name for the spotcam power service.
    static const char* s_default_service_name;

    // Default service type for the spotcam power service.
    static const char* s_service_type;
};

}  // namespace spot_cam
}  // namespace client

}  // namespace bosdyn
