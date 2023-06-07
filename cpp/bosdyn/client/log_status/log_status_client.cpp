/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "log_status_client.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* LogStatusClient::s_default_service_name = "log-status";

const char* LogStatusClient::s_service_type = "bosdyn.api.log_status.LogStatusService";

std::shared_future<GetLogStatusResponseType> LogStatusClient::GetLogStatusAsync(
    ::bosdyn::api::log_status::GetLogStatusRequest& request, const RPCParameters& parameters) {
    std::promise<GetLogStatusResponseType> response;
    std::shared_future<GetLogStatusResponseType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<bosdyn::api::log_status::GetLogStatusRequest,
                          bosdyn::api::log_status::GetLogStatusResponse,
                          ::bosdyn::api::log_status::GetLogStatusResponse>(
            request,
            std::bind(&::bosdyn::api::log_status::LogStatusService::Stub::AsyncGetLogStatus,
                      m_stub.get(), _1, _2, _3),
            std::bind(&LogStatusClient::OnGetLogStatusComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

GetLogStatusResponseType LogStatusClient::GetLogStatus(
    ::bosdyn::api::log_status::GetLogStatusRequest& request, const RPCParameters& parameters) {
    return GetLogStatusAsync(request, parameters).get();
}

std::shared_future<GetActiveLogStatusesResponseType> LogStatusClient::GetActiveLogStatusesAsync(
    ::bosdyn::api::log_status::GetActiveLogStatusesRequest& request,
    const RPCParameters& parameters) {
    std::promise<GetActiveLogStatusesResponseType> response;
    std::shared_future<GetActiveLogStatusesResponseType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<bosdyn::api::log_status::GetActiveLogStatusesRequest,
                          bosdyn::api::log_status::GetActiveLogStatusesResponse,
                          ::bosdyn::api::log_status::GetActiveLogStatusesResponse>(
            request,
            std::bind(&::bosdyn::api::log_status::LogStatusService::Stub::AsyncGetActiveLogStatuses,
                      m_stub.get(), _1, _2, _3),
            std::bind(&LogStatusClient::OnGetActiveLogStatusesComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

GetActiveLogStatusesResponseType LogStatusClient::GetActiveLogStatuses(
    ::bosdyn::api::log_status::GetActiveLogStatusesRequest& request,
    const RPCParameters& parameters) {
    return GetActiveLogStatusesAsync(request, parameters).get();
}

std::shared_future<StartExperimentLogResponseType> LogStatusClient::StartExperimentLogAsync(
        ::bosdyn::api::log_status::StartExperimentLogRequest& request,
        const RPCParameters& parameters) {
    std::promise<StartExperimentLogResponseType> response;
    std::shared_future<StartExperimentLogResponseType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
            InitiateAsyncCall<bosdyn::api::log_status::StartExperimentLogRequest,
                    bosdyn::api::log_status::StartExperimentLogResponse,
                    ::bosdyn::api::log_status::StartExperimentLogResponse>(
                    request,
                    std::bind(&::bosdyn::api::log_status::LogStatusService::Stub::AsyncStartExperimentLog,
                              m_stub.get(), _1, _2, _3),
                    std::bind(&LogStatusClient::OnStartExperimentLogComplete, this, _1, _2, _3, _4, _5),
                    std::move(response), parameters);

    return future;
}

StartExperimentLogResponseType LogStatusClient::StartExperimentLog(
        ::bosdyn::api::log_status::StartExperimentLogRequest& request,
        const RPCParameters& parameters) {
    return StartExperimentLogAsync(request, parameters).get();
}

std::shared_future<StartRetroLogResponseType> LogStatusClient::StartRetroLogAsync(
    ::bosdyn::api::log_status::StartRetroLogRequest& request,
    const RPCParameters& parameters) {
    std::promise<StartRetroLogResponseType> response;
    std::shared_future<StartRetroLogResponseType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<bosdyn::api::log_status::StartRetroLogRequest,
                          bosdyn::api::log_status::StartRetroLogResponse,
                          ::bosdyn::api::log_status::StartRetroLogResponse>(
            request,
            std::bind(&::bosdyn::api::log_status::LogStatusService::Stub::AsyncStartRetroLog,
                      m_stub.get(), _1, _2, _3),
            std::bind(&LogStatusClient::OnStartRetroLogComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

StartRetroLogResponseType LogStatusClient::StartRetroLog(
    ::bosdyn::api::log_status::StartRetroLogRequest& request,
    const RPCParameters& parameters) {
    return StartRetroLogAsync(request, parameters).get();
}

std::shared_future<UpdateExperimentLogResponseType> LogStatusClient::UpdateExperimentLogAsync(
    ::bosdyn::api::log_status::UpdateExperimentLogRequest& request,
    const RPCParameters& parameters) {
    std::promise<UpdateExperimentLogResponseType> response;
    std::shared_future<UpdateExperimentLogResponseType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<bosdyn::api::log_status::UpdateExperimentLogRequest,
                          bosdyn::api::log_status::UpdateExperimentLogResponse,
                          ::bosdyn::api::log_status::UpdateExperimentLogResponse>(
            request,
            std::bind(&::bosdyn::api::log_status::LogStatusService::Stub::AsyncUpdateExperimentLog,
                      m_stub.get(), _1, _2, _3),
            std::bind(&LogStatusClient::OnUpdateExperimentLogComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

UpdateExperimentLogResponseType LogStatusClient::UpdateExperimentLog(
    ::bosdyn::api::log_status::UpdateExperimentLogRequest& request,
    const RPCParameters& parameters) {
    return UpdateExperimentLogAsync(request, parameters).get();
}

std::shared_future<TerminateLogResponseType> LogStatusClient::TerminateLogAsync(
    ::bosdyn::api::log_status::TerminateLogRequest& request, const RPCParameters& parameters) {
    std::promise<TerminateLogResponseType> response;
    std::shared_future<TerminateLogResponseType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<bosdyn::api::log_status::TerminateLogRequest,
                          bosdyn::api::log_status::TerminateLogResponse,
                          ::bosdyn::api::log_status::TerminateLogResponse>(
            request,
            std::bind(&::bosdyn::api::log_status::LogStatusService::Stub::AsyncTerminateLog,
                      m_stub.get(), _1, _2, _3),
            std::bind(&LogStatusClient::OnTerminateLogComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

TerminateLogResponseType LogStatusClient::TerminateLog(
    ::bosdyn::api::log_status::TerminateLogRequest& request, const RPCParameters& parameters) {
    return TerminateLogAsync(request, parameters).get();
}

void LogStatusClient::OnGetLogStatusComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::log_status::GetLogStatusRequest& request,
    ::bosdyn::api::log_status::GetLogStatusResponse&& response, const grpc::Status& status,
    std::promise<GetLogStatusResponseType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseAndGetFinalStatus<::bosdyn::api::log_status::GetLogStatusResponse>(
            status, response, response.status());
    promise.set_value({ret_status, std::move(response)});
}

void LogStatusClient::OnGetActiveLogStatusesComplete(
    MessagePumpCallBase* call,
    const ::bosdyn::api::log_status::GetActiveLogStatusesRequest& request,
    ::bosdyn::api::log_status::GetActiveLogStatusesResponse&& response, const grpc::Status& status,
    std::promise<GetActiveLogStatusesResponseType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseAndGetFinalStatus<::bosdyn::api::log_status::GetActiveLogStatusesResponse>(
            status, response, response.status());
    promise.set_value({ret_status, std::move(response)});
}

void LogStatusClient::OnStartExperimentLogComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::log_status::StartExperimentLogRequest& request,
        ::bosdyn::api::log_status::StartExperimentLogResponse&& response, const grpc::Status& status,
        std::promise<StartExperimentLogResponseType> promise) {
    ::bosdyn::common::Status ret_status =
            ProcessResponseAndGetFinalStatus<::bosdyn::api::log_status::StartExperimentLogResponse>(
                    status, response, response.status());
    promise.set_value({ret_status, std::move(response)});
}

void LogStatusClient::OnStartRetroLogComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::log_status::StartRetroLogRequest& request,
    ::bosdyn::api::log_status::StartRetroLogResponse&& response, const grpc::Status& status,
    std::promise<StartRetroLogResponseType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseAndGetFinalStatus<::bosdyn::api::log_status::StartRetroLogResponse>(
            status, response, response.status());
    promise.set_value({ret_status, std::move(response)});
}

void LogStatusClient::OnUpdateExperimentLogComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::log_status::UpdateExperimentLogRequest& request,
    ::bosdyn::api::log_status::UpdateExperimentLogResponse&& response, const grpc::Status& status,
    std::promise<UpdateExperimentLogResponseType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseAndGetFinalStatus<::bosdyn::api::log_status::UpdateExperimentLogResponse>(
            status, response, response.status());
    promise.set_value({ret_status, std::move(response)});
}

void LogStatusClient::OnTerminateLogComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::log_status::TerminateLogRequest& request,
    ::bosdyn::api::log_status::TerminateLogResponse&& response, const grpc::Status& status,
    std::promise<TerminateLogResponseType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseAndGetFinalStatus<::bosdyn::api::log_status::TerminateLogResponse>(
            status, response, response.status());
    promise.set_value({ret_status, std::move(response)});
}

ServiceClient::QualityOfService LogStatusClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void LogStatusClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::log_status::LogStatusService::Stub(channel));
}

}  // namespace client

}  // namespace bosdyn
