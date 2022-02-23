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
#include "bosdyn/client/spot_cam/media_log/media_log_error_codes.h"

#include <bosdyn/api/spot_cam/service.grpc.pb.h>
#include <bosdyn/api/spot_cam/service.pb.h>
#include <future>

namespace bosdyn {

namespace client {

namespace spot_cam {

typedef Result<::bosdyn::api::spot_cam::StoreResponse> StoreResultType;
typedef Result<::bosdyn::api::spot_cam::GetStatusResponse> GetStatusResultType;
typedef Result<::bosdyn::api::spot_cam::TagResponse> TagResultType;
typedef Result<::bosdyn::api::spot_cam::DebugResponse> DebugResultType;
typedef Result<::bosdyn::api::spot_cam::ListCamerasResponse> ListCamerasResultType;
typedef Result<::bosdyn::api::spot_cam::DeleteResponse> DeleteResultType;

typedef Result<::bosdyn::api::spot_cam::RetrieveRawDataResponse> RetrieveRawDataResultType;
typedef Result<::bosdyn::api::spot_cam::RetrieveResponse> RetrieveResultType;
typedef Result<::bosdyn::api::spot_cam::ListLogpointsResponse> ListLogpointsResultType;

class MediaLogClient : public ServiceClient {
 public:
    MediaLogClient() = default;

    ~MediaLogClient() = default;

    std::shared_future<StoreResultType> StoreAsync(
        const std::string& camera_name, const std::string& tag,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<StoreResultType> StoreAsync(
        const std::string& camera_name,
        const RPCParameters& parameters = RPCParameters());

    StoreResultType Store(
        const std::string& camera_name, const std::string& tag,
        const RPCParameters& parameters = RPCParameters());

    StoreResultType Store(
        const std::string& camera_name,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<StoreResultType> StoreAsync(
        ::bosdyn::api::spot_cam::StoreRequest& request,
        const RPCParameters& parameters = RPCParameters());

    StoreResultType Store(
        ::bosdyn::api::spot_cam::StoreRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetStatusResultType> GetStatusAsync(
        const std::string& logpoint_name,
        const RPCParameters& parameters = RPCParameters());

    GetStatusResultType GetStatus(
        const std::string& logpoint_name,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetStatusResultType> GetStatusAsync(
        ::bosdyn::api::spot_cam::GetStatusRequest& request,
        const RPCParameters& parameters = RPCParameters());
    GetStatusResultType GetStatus(
        ::bosdyn::api::spot_cam::GetStatusRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<TagResultType> TagAsync(
        const std::string& logpoint_name, const std::string& tag,
        const RPCParameters& parameters = RPCParameters());

    TagResultType Tag(
        const std::string& logpoint_name, const std::string& tag,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<TagResultType> TagAsync(
        ::bosdyn::api::spot_cam::TagRequest& request,
        const RPCParameters& parameters = RPCParameters());

    TagResultType Tag(
        ::bosdyn::api::spot_cam::TagRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<DebugResultType> EnableDebugAsync(
        ::bosdyn::api::spot_cam::DebugRequest& request,
        const RPCParameters& parameters = RPCParameters());

    DebugResultType EnableDebug(
        ::bosdyn::api::spot_cam::DebugRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<ListCamerasResultType> ListCamerasAsync(
        const RPCParameters& parameters = RPCParameters());

    ListCamerasResultType ListCameras(
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<DeleteResultType> DeleteAsync(
        const std::string& logpoint_name,
        const RPCParameters& parameters = RPCParameters());

    DeleteResultType Delete(
        const std::string& logpoint_name,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<DeleteResultType> DeleteAsync(
        ::bosdyn::api::spot_cam::DeleteRequest& request,
        const RPCParameters& parameters = RPCParameters());

    DeleteResultType Delete(
        ::bosdyn::api::spot_cam::DeleteRequest& request,
        const RPCParameters& parameters = RPCParameters());


    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the spot cam media log service will be registered in the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the spot cam media log service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous grpc calls.
    void OnStoreComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::StoreRequest& request,
        ::bosdyn::api::spot_cam::StoreResponse&& response, const grpc::Status& status,
        std::promise<StoreResultType> promise);
    void OnGetStatusComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::GetStatusRequest& request,
        ::bosdyn::api::spot_cam::GetStatusResponse&& response, const grpc::Status& status,
        std::promise<GetStatusResultType> promise);
    void OnTagComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::TagRequest& request,
        ::bosdyn::api::spot_cam::TagResponse&& response, const grpc::Status& status,
        std::promise<TagResultType> promise);
    void OnEnableDebugComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::DebugRequest& request,
        ::bosdyn::api::spot_cam::DebugResponse&& response, const grpc::Status& status,
        std::promise<DebugResultType> promise);
    void OnListCamerasComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::ListCamerasRequest& request,
        ::bosdyn::api::spot_cam::ListCamerasResponse&& response, const grpc::Status& status,
        std::promise<ListCamerasResultType> promise);
    void OnDeleteComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::DeleteRequest& request,
        ::bosdyn::api::spot_cam::DeleteResponse&& response, const grpc::Status& status,
        std::promise<DeleteResultType> promise);


    std::unique_ptr<::bosdyn::api::spot_cam::MediaLogService::Stub> m_stub;

    // Default service name for the spotcam media log service.
    static const char* s_default_service_name;

    // Default service type for the spotcam media log service.
    static const char* s_service_type;
};

}  // namespace spot_cam
}  // namespace client

}  // namespace bosdyn
