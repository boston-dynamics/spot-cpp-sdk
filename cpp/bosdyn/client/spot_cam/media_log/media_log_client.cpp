/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/spot_cam/media_log/media_log_client.h"
#include "bosdyn/common/success_condition.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

namespace spot_cam {

const char* MediaLogClient::s_default_service_name = "spot-cam-media-log";

const char* MediaLogClient::s_service_type = "bosdyn.api.spot_cam.MediaLogService";

std::shared_future<StoreResultType> MediaLogClient::StoreAsync(
    const std::string& camera_name, const std::string& tag,
    const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::StoreRequest request;

    request.mutable_camera()->set_name(camera_name);
    request.set_tag(camera_name);
    request.set_type(::bosdyn::api::spot_cam::Logpoint::STILLIMAGE);

    return StoreAsync(request, parameters);
}

std::shared_future<StoreResultType> MediaLogClient::StoreAsync(
    const std::string& camera_name, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::StoreRequest request;

    request.mutable_camera()->set_name(camera_name);
    request.set_type(::bosdyn::api::spot_cam::Logpoint::STILLIMAGE);

    return StoreAsync(request, parameters);
}

StoreResultType MediaLogClient::Store(
    const std::string& camera_name, const std::string& tag,
    const RPCParameters& parameters) {
    return StoreAsync(camera_name, tag, parameters).get();
}

StoreResultType MediaLogClient::Store(
    const std::string& camera_name, const RPCParameters& parameters) {
    return StoreAsync(camera_name, parameters).get();
}

std::shared_future<StoreResultType> MediaLogClient::StoreAsync(
    ::bosdyn::api::spot_cam::StoreRequest& request, const RPCParameters& parameters) {
    std::promise<StoreResultType> response;
    std::shared_future<StoreResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::StoreRequest, ::bosdyn::api::spot_cam::StoreResponse,
                          ::bosdyn::api::spot_cam::StoreResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::MediaLogService::Stub::AsyncStore, m_stub.get(), _1, _2, _3),
            std::bind(&MediaLogClient::OnStoreComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

StoreResultType MediaLogClient::Store(
    ::bosdyn::api::spot_cam::StoreRequest& request, const RPCParameters& parameters) {
    return StoreAsync(request, parameters).get();
}

void MediaLogClient::OnStoreComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::StoreRequest& request,
    ::bosdyn::api::spot_cam::StoreResponse&& response, const grpc::Status& status,
    std::promise<StoreResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::StoreResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<GetStatusResultType> MediaLogClient::GetStatusAsync(
    const std::string& logpoint_name, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::GetStatusRequest request;

    request.mutable_point()->set_name(logpoint_name);

    return GetStatusAsync(request, parameters);
}

GetStatusResultType MediaLogClient::GetStatus(
    const std::string& logpoint_name, const RPCParameters& parameters) {
    return GetStatusAsync(logpoint_name, parameters).get();
}

std::shared_future<GetStatusResultType> MediaLogClient::GetStatusAsync(
    ::bosdyn::api::spot_cam::GetStatusRequest& request,
    const RPCParameters& parameters) {
    std::promise<GetStatusResultType> response;
    std::shared_future<GetStatusResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::GetStatusRequest, ::bosdyn::api::spot_cam::GetStatusResponse,
                          ::bosdyn::api::spot_cam::GetStatusResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::MediaLogService::Stub::AsyncGetStatus, m_stub.get(), _1, _2,
                      _3),
            std::bind(&MediaLogClient::OnGetStatusComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

GetStatusResultType MediaLogClient::GetStatus(
    ::bosdyn::api::spot_cam::GetStatusRequest& request,
    const RPCParameters& parameters) {
    return GetStatusAsync(request, parameters).get();
}

void MediaLogClient::OnGetStatusComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::GetStatusRequest& request,
    ::bosdyn::api::spot_cam::GetStatusResponse&& response, const grpc::Status& status,
    std::promise<GetStatusResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::GetStatusResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<TagResultType> MediaLogClient::TagAsync(
    const std::string& logpoint_name, const std::string& tag,
    const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::TagRequest request;

    request.mutable_point()->set_name(logpoint_name);
    request.mutable_point()->set_tag(tag);

    return TagAsync(request, parameters);
}

TagResultType MediaLogClient::Tag(
    const std::string& logpoint_name, const std::string& tag,
    const RPCParameters& parameters) {
    return TagAsync(logpoint_name, tag, parameters).get();
}

std::shared_future<TagResultType> MediaLogClient::TagAsync(
    ::bosdyn::api::spot_cam::TagRequest& request, const RPCParameters& parameters) {
    std::promise<TagResultType> response;
    std::shared_future<TagResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::TagRequest, ::bosdyn::api::spot_cam::TagResponse,
                          ::bosdyn::api::spot_cam::TagResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::MediaLogService::Stub::AsyncTag, m_stub.get(), _1, _2, _3),
            std::bind(&MediaLogClient::OnTagComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

TagResultType MediaLogClient::Tag(
    ::bosdyn::api::spot_cam::TagRequest& request, const RPCParameters& parameters) {
    return TagAsync(request, parameters).get();
}

void MediaLogClient::OnTagComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::TagRequest& request,
    ::bosdyn::api::spot_cam::TagResponse&& response, const grpc::Status& status,
    std::promise<TagResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::TagResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<DebugResultType> MediaLogClient::EnableDebugAsync(
    ::bosdyn::api::spot_cam::DebugRequest& request, const RPCParameters& parameters) {
    std::promise<DebugResultType> response;
    std::shared_future<DebugResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::DebugRequest, ::bosdyn::api::spot_cam::DebugResponse,
                          ::bosdyn::api::spot_cam::DebugResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::MediaLogService::Stub::AsyncEnableDebug, m_stub.get(), _1, _2, _3),
            std::bind(&MediaLogClient::OnEnableDebugComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

DebugResultType MediaLogClient::EnableDebug(
    ::bosdyn::api::spot_cam::DebugRequest& request, const RPCParameters& parameters) {
    return EnableDebugAsync(request, parameters).get();
}

void MediaLogClient::OnEnableDebugComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::DebugRequest& request,
    ::bosdyn::api::spot_cam::DebugResponse&& response,const grpc::Status& status,
    std::promise<DebugResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::DebugResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<ListCamerasResultType> MediaLogClient::ListCamerasAsync(
    const RPCParameters& parameters) {
    std::promise<ListCamerasResultType> response;
    std::shared_future<ListCamerasResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::spot_cam::ListCamerasRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::ListCamerasRequest, ::bosdyn::api::spot_cam::ListCamerasResponse,
                          ::bosdyn::api::spot_cam::ListCamerasResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::MediaLogService::Stub::AsyncListCameras, m_stub.get(), _1, _2,
                      _3),
            std::bind(&MediaLogClient::OnListCamerasComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

ListCamerasResultType MediaLogClient::ListCameras(
    const RPCParameters& parameters) {
    return ListCamerasAsync(parameters).get(); }

void MediaLogClient::OnListCamerasComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::ListCamerasRequest& request,
    ::bosdyn::api::spot_cam::ListCamerasResponse&& response, const grpc::Status& status,
    std::promise<ListCamerasResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::ListCamerasResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<DeleteResultType> MediaLogClient::DeleteAsync(
    const std::string& logpoint_name, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::DeleteRequest request;

    request.mutable_point()->set_name(logpoint_name);

    return DeleteAsync(request, parameters);
}

DeleteResultType MediaLogClient::Delete(
    const std::string& logpoint_name, const RPCParameters& parameters) {
    return DeleteAsync(logpoint_name, parameters).get();
}

std::shared_future<DeleteResultType> MediaLogClient::DeleteAsync(
    ::bosdyn::api::spot_cam::DeleteRequest& request, const RPCParameters& parameters) {
    std::promise<DeleteResultType> response;
    std::shared_future<DeleteResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::DeleteRequest, ::bosdyn::api::spot_cam::DeleteResponse,
                          ::bosdyn::api::spot_cam::DeleteResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::MediaLogService::Stub::AsyncDelete, m_stub.get(), _1, _2, _3),
            std::bind(&MediaLogClient::OnDeleteComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

DeleteResultType MediaLogClient::Delete(
    ::bosdyn::api::spot_cam::DeleteRequest& request, const RPCParameters& parameters) {
    return DeleteAsync(request, parameters).get();
}

void MediaLogClient::OnDeleteComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::DeleteRequest& request,
    ::bosdyn::api::spot_cam::DeleteResponse&& response, const grpc::Status& status,
    std::promise<DeleteResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::DeleteResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

ServiceClient::QualityOfService MediaLogClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void MediaLogClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::spot_cam::MediaLogService::Stub(channel));
}

}  // namespace spot_cam
}  // namespace client

}  // namespace bosdyn
