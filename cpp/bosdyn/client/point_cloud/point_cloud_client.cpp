/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/point_cloud/point_cloud_client.h"
#include "bosdyn/common/success_condition.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* PointCloudClient::s_default_service_name = "velodyne-point-cloud";

const char* PointCloudClient::s_service_type = "bosdyn.api.PointCloudService";

std::shared_future<ListPointCloudSourcesResultType> PointCloudClient::ListPointCloudSourcesAsync(
    const RPCParameters& parameters) {
    std::promise<ListPointCloudSourcesResultType> response;
    std::shared_future<ListPointCloudSourcesResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::ListPointCloudSourcesRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::ListPointCloudSourcesRequest, ::bosdyn::api::ListPointCloudSourcesResponse,
                          ::bosdyn::api::ListPointCloudSourcesResponse>(
            request,
            std::bind(&::bosdyn::api::PointCloudService::Stub::AsyncListPointCloudSources, m_stub.get(), _1,
                      _2, _3),
            std::bind(&PointCloudClient::OnListSourcesComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

ListPointCloudSourcesResultType PointCloudClient::ListPointCloudSources(
    const RPCParameters& parameters) {
    return ListPointCloudSourcesAsync(parameters).get();
}

void PointCloudClient::OnListSourcesComplete(
    MessagePumpCallBase*, const ::bosdyn::api::ListPointCloudSourcesRequest&,
    ::bosdyn::api::ListPointCloudSourcesResponse&& response, const grpc::Status& status,
    std::promise<ListPointCloudSourcesResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::ListPointCloudSourcesResponse>(
        status, response, SDKErrorCode::Success);
    if (!ret_status) {
        promise.set_value({ret_status, std::move(response)});
        return;
    }

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<GetPointCloudResultType> PointCloudClient::GetPointCloudAsync(
    const std::vector<std::string>& point_cloud_sources,
    const RPCParameters& parameters) {
    std::promise<GetPointCloudResultType> error_response;
    std::shared_future<GetPointCloudResultType> error_future = error_response.get_future();

    ::bosdyn::api::GetPointCloudRequest request;
    for (const std::string& source : point_cloud_sources) {
        ::bosdyn::api::PointCloudRequest* proto_point_cloud_request = request.add_point_cloud_requests();
        BuildPointCloudRequest(source, proto_point_cloud_request);
    }

    return GetPointCloudAsync(request, parameters);
}

GetPointCloudResultType PointCloudClient::GetPointCloud(
    const std::vector<std::string>& point_cloud_sources,
    const RPCParameters& parameters) {
    return GetPointCloudAsync(point_cloud_sources, parameters).get();
}

std::shared_future<GetPointCloudResultType> PointCloudClient::GetPointCloudAsync(
    ::bosdyn::api::GetPointCloudRequest& request,
    const RPCParameters& parameters) {
    std::promise<GetPointCloudResultType> response;
    std::shared_future<GetPointCloudResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::GetPointCloudRequest, ::bosdyn::api::GetPointCloudResponse,
                          ::bosdyn::api::GetPointCloudResponse>(
            request,
            std::bind(&::bosdyn::api::PointCloudService::Stub::AsyncGetPointCloud, m_stub.get(), _1, _2, _3),
            std::bind(&PointCloudClient::OnGetPointCloudComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

GetPointCloudResultType PointCloudClient::GetPointCloud(
    ::bosdyn::api::GetPointCloudRequest& request,
    const RPCParameters& parameters) {
    return GetPointCloudAsync(request, parameters).get();
}

void PointCloudClient::OnGetPointCloudComplete(
    MessagePumpCallBase*, const ::bosdyn::api::GetPointCloudRequest& request,
    ::bosdyn::api::GetPointCloudResponse&& response, const grpc::Status& status,
    std::promise<GetPointCloudResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::GetPointCloudResponse>(
        status, response, SDKErrorCode::Success);

    if (!ret_status) {
        promise.set_value({ret_status, std::move(response)});
        return;
    }

    // Iterate through the PointCloudResponse.status to verify there are no errors.
    for (const auto& point_cloud_response : response.point_cloud_responses()) {
        std::error_code code = point_cloud_response.status();
        if (code != SuccessCondition::Success) {
            promise.set_value({::bosdyn::common::Status(code, "PointCloudResponse ::bosdyn::common::Status unsuccessful"),
                               std::move(response)});
            return;
        }
    }

    promise.set_value({ret_status, std::move(response)});
}

void PointCloudClient::BuildPointCloudRequest(
    const std::string& point_cloud_source_name,
    ::bosdyn::api::PointCloudRequest* request) {
    request->set_point_cloud_source_name(point_cloud_source_name);
}

ServiceClient::QualityOfService PointCloudClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void PointCloudClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::PointCloudService::Stub(channel));
}

}  // namespace client

}  // namespace bosdyn
