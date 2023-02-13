/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/spot_cam/service.grpc.pb.h>
#include <bosdyn/api/spot_cam/service.pb.h>

#include <future>
#include <memory>
#include <string>
#include <vector>

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/client/spot_cam/health/health_error_codes.h"

namespace bosdyn {

namespace client {

namespace spot_cam {

typedef Result<::bosdyn::api::spot_cam::ClearBITEventsResponse> ClearBITEventsResultType;
typedef Result<::bosdyn::api::spot_cam::GetBITStatusResponse> GetBITStatusResultType;
typedef Result<::bosdyn::api::spot_cam::GetTemperatureResponse> GetTemperatureResultType;
// SpotCAM returns an ascii armored copy of its system log, we represent that as a string.
typedef Result<std::string> GetSystemLogResultType;

class HealthClient : public ServiceClient {
 public:
    HealthClient() = default;

    ~HealthClient() = default;

    std::shared_future<ClearBITEventsResultType> ClearBITEventsAsync(
        const RPCParameters& parameters = RPCParameters());

    ClearBITEventsResultType ClearBITEvents(const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetBITStatusResultType> GetBITStatusAsync(
        const RPCParameters& parameters = RPCParameters());
    GetBITStatusResultType GetBITStatus(const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetTemperatureResultType> GetTemperatureAsync(
        const RPCParameters& parameters = RPCParameters());
    GetTemperatureResultType GetTemperature(const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetSystemLogResultType> GetSystemLogAsync(
        const RPCParameters& parameters = RPCParameters());
    GetSystemLogResultType GetSystemLog(const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the spot cam health service will be registered in the directory
    //  with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the spot cam health service that will be registered in the
    //  directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous grpc calls.
    void OnClearBITEventsComplete(MessagePumpCallBase* call,
                                  const ::bosdyn::api::spot_cam::ClearBITEventsRequest& request,
                                  ::bosdyn::api::spot_cam::ClearBITEventsResponse&& response,
                                  const grpc::Status& status,
                                  std::promise<ClearBITEventsResultType> promise);
    void OnGetBITStatusComplete(MessagePumpCallBase* call,
                                const ::bosdyn::api::spot_cam::GetBITStatusRequest& request,
                                ::bosdyn::api::spot_cam::GetBITStatusResponse&& response,
                                const grpc::Status& status,
                                std::promise<GetBITStatusResultType> promise);
    void OnGetTemperatureComplete(MessagePumpCallBase* call,
                                  const ::bosdyn::api::spot_cam::GetTemperatureRequest& request,
                                  ::bosdyn::api::spot_cam::GetTemperatureResponse&& response,
                                  const grpc::Status& status,
                                  std::promise<GetTemperatureResultType> promise);
    void OnGetSystemLogComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::GetSystemLogRequest& request,
        std::vector<::bosdyn::api::spot_cam::GetSystemLogResponse>&& responses,
        const grpc::Status& status, std::promise<GetSystemLogResultType> promise);

    std::unique_ptr<::bosdyn::api::spot_cam::HealthService::Stub> m_stub;

    // Default service name for the spot cam health service.
    static const char* s_default_service_name;

    // Default service type for the spotcam health service.
    static const char* s_service_type;
};

}  // namespace spot_cam
}  // namespace client

}  // namespace bosdyn
