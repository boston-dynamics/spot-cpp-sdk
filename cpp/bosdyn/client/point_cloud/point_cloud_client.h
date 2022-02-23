/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/point_cloud_service.grpc.pb.h>
#include <bosdyn/api/point_cloud_service.pb.h>

#include <future>
#include <string>
#include <vector>

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/client/point_cloud/point_cloud_error_codes.h"

namespace bosdyn {

namespace client {

typedef Result<::bosdyn::api::ListPointCloudSourcesResponse> ListPointCloudSourcesResultType;
typedef Result<::bosdyn::api::GetPointCloudResponse> GetPointCloudResultType;

class PointCloudClient : public ServiceClient {
 public:
    PointCloudClient() = default;

    ~PointCloudClient() = default;

    // Asynchronous method to get a ListPointCloudSourcesResponse.
    std::shared_future<ListPointCloudSourcesResultType> ListPointCloudSourcesAsync(
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to get a ListPointCloudSourcesResponse.
    ListPointCloudSourcesResultType ListPointCloudSources(
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to get a GetPointCloudResponse for a list of point cloud sources.
    std::shared_future<GetPointCloudResultType> GetPointCloudAsync(
        const std::vector<std::string>& point_cloud_sources,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to get a GetPointCloudResponse for a list of point cloud sources.
    GetPointCloudResultType GetPointCloud(
        const std::vector<std::string>& point_cloud_sources,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to get a GetPointCloudResponse for the given GetPointCloudRequest.
    // request is not const because the method updates the header.
    std::shared_future<GetPointCloudResultType> GetPointCloudAsync(
        ::bosdyn::api::GetPointCloudRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to get a GetPointCloudResponse for the given GetPointCloudRequest.
    // request is not const because the method updates the header.
    GetPointCloudResultType GetPointCloud(
        ::bosdyn::api::GetPointCloudRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the PointCloud service will be registered in the directory
    // with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the PointCloud service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous ListSources calls.
    void OnListSourcesComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::ListPointCloudSourcesRequest& request,
        ::bosdyn::api::ListPointCloudSourcesResponse&& response, const grpc::Status& status,
        std::promise<ListPointCloudSourcesResultType> promise);
    // Callback function registered for the asynchronous GetPointCloud calls.
    void OnGetPointCloudComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::GetPointCloudRequest& request,
        ::bosdyn::api::GetPointCloudResponse&& response, const grpc::Status& status,
        std::promise<GetPointCloudResultType> promise);

    void BuildPointCloudRequest(
        const std::string& point_cloud_source_name,
        ::bosdyn::api::PointCloudRequest* request);

    // Comms details.
    std::unique_ptr<::bosdyn::api::PointCloudService::Stub> m_stub;

    // Default service name for the PointCloud service.
    static const char* s_default_service_name;

    // Default service type for the PointCloud service.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
