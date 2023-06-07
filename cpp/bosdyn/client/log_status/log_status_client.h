/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <future>

#include <bosdyn/api/log_status/log_status_service.grpc.pb.h>

#include "bosdyn/client/log_status/log_status_error_codes.h"
#include "bosdyn/client/service_client/service_client.h"

namespace bosdyn {

namespace client {

typedef Result<::bosdyn::api::log_status::GetLogStatusResponse> GetLogStatusResponseType;
typedef Result<::bosdyn::api::log_status::GetActiveLogStatusesResponse> GetActiveLogStatusesResponseType;
typedef Result<::bosdyn::api::log_status::StartExperimentLogResponse>
    StartExperimentLogResponseType;
typedef Result<::bosdyn::api::log_status::StartRetroLogResponse>
        StartRetroLogResponseType;
typedef Result<::bosdyn::api::log_status::UpdateExperimentLogResponse>
    UpdateExperimentLogResponseType;
typedef Result<::bosdyn::api::log_status::TerminateLogResponse> TerminateLogResponseType;

class LogStatusClient : public ServiceClient {
 public:
    LogStatusClient() = default;

    ~LogStatusClient() = default;

    std::shared_future<GetLogStatusResponseType> GetLogStatusAsync(
        ::bosdyn::api::log_status::GetLogStatusRequest& request,
        const RPCParameters& parameters = RPCParameters());

    GetLogStatusResponseType GetLogStatus(
        ::bosdyn::api::log_status::GetLogStatusRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetActiveLogStatusesResponseType> GetActiveLogStatusesAsync(
            ::bosdyn::api::log_status::GetActiveLogStatusesRequest& request,
            const RPCParameters& parameters = RPCParameters());

    GetActiveLogStatusesResponseType GetActiveLogStatuses(
            ::bosdyn::api::log_status::GetActiveLogStatusesRequest& request,
            const RPCParameters& parameters = RPCParameters());

    std::shared_future<StartExperimentLogResponseType> StartExperimentLogAsync(
        ::bosdyn::api::log_status::StartExperimentLogRequest& request,
        const RPCParameters& parameters = RPCParameters());

    StartExperimentLogResponseType StartExperimentLog(
        ::bosdyn::api::log_status::StartExperimentLogRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<StartRetroLogResponseType> StartRetroLogAsync(
            ::bosdyn::api::log_status::StartRetroLogRequest& request,
            const RPCParameters& parameters = RPCParameters());

    StartRetroLogResponseType StartRetroLog(
            ::bosdyn::api::log_status::StartRetroLogRequest& request,
            const RPCParameters& parameters = RPCParameters());

    std::shared_future<UpdateExperimentLogResponseType> UpdateExperimentLogAsync(
        ::bosdyn::api::log_status::UpdateExperimentLogRequest& request,
        const RPCParameters& parameters = RPCParameters());

    UpdateExperimentLogResponseType UpdateExperimentLog(
        ::bosdyn::api::log_status::UpdateExperimentLogRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<TerminateLogResponseType> TerminateLogAsync(
        ::bosdyn::api::log_status::TerminateLogRequest& request,
        const RPCParameters& parameters = RPCParameters());

    TerminateLogResponseType TerminateLog(::bosdyn::api::log_status::TerminateLogRequest& request,
                                          const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the LogsBundling service will be registered in the directory
    // with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the LogsBundling service that will be registered in the
    // directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // GRPC async callbacks

    void OnGetLogStatusComplete(MessagePumpCallBase* call,
                                const ::bosdyn::api::log_status::GetLogStatusRequest& request,
                                ::bosdyn::api::log_status::GetLogStatusResponse&& response,
                                const grpc::Status& status,
                                std::promise<GetLogStatusResponseType> promise);

    void OnGetActiveLogStatusesComplete(MessagePumpCallBase* call,
                                const ::bosdyn::api::log_status::GetActiveLogStatusesRequest& request,
                                ::bosdyn::api::log_status::GetActiveLogStatusesResponse&& response,
                                const grpc::Status& status,
                                std::promise<GetActiveLogStatusesResponseType> promise);

    void OnStartExperimentLogComplete(
        MessagePumpCallBase* call,
        const ::bosdyn::api::log_status::StartExperimentLogRequest& request,
        ::bosdyn::api::log_status::StartExperimentLogResponse&& response,
        const grpc::Status& status, std::promise<StartExperimentLogResponseType> promise);

    void OnStartRetroLogComplete(
            MessagePumpCallBase* call,
            const ::bosdyn::api::log_status::StartRetroLogRequest& request,
            ::bosdyn::api::log_status::StartRetroLogResponse&& response,
            const grpc::Status& status, std::promise<StartRetroLogResponseType> promise);

    void OnUpdateExperimentLogComplete(
        MessagePumpCallBase* call,
        const ::bosdyn::api::log_status::UpdateExperimentLogRequest& request,
        ::bosdyn::api::log_status::UpdateExperimentLogResponse&& response,
        const grpc::Status& status, std::promise<UpdateExperimentLogResponseType> promise);

    void OnTerminateLogComplete(MessagePumpCallBase* call,
                                const ::bosdyn::api::log_status::TerminateLogRequest& request,
                                ::bosdyn::api::log_status::TerminateLogResponse&& response,
                                const grpc::Status& status,
                                std::promise<TerminateLogResponseType> promise);

    std::unique_ptr<::bosdyn::api::log_status::LogStatusService::Stub> m_stub;

    // Default service name for the LogsBundling service.
    static const char* s_default_service_name;

    // Default service type for the LogsBundling service.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
