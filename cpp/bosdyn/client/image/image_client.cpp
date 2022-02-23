/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "image_client.h"

#include <memory>
#include <string>
#include <vector>

#include "bosdyn/common/success_condition.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* ImageClient::s_default_service_name = "image";

const char* ImageClient::s_service_type = "bosdyn.api.ImageService";

std::shared_future<ImageListSourcesResultType> ImageClient::ListImageSourcesAsync(
    const RPCParameters& parameters) {
    std::promise<ImageListSourcesResultType> response;
    std::shared_future<ImageListSourcesResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::ListImageSourcesRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::ListImageSourcesRequest, ::bosdyn::api::ListImageSourcesResponse,
                          ::bosdyn::api::ListImageSourcesResponse>(
            request,
            std::bind(&::bosdyn::api::ImageService::Stub::AsyncListImageSources, m_stub.get(), _1, _2, _3),
            std::bind(&ImageClient::OnListSourcesComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

ImageListSourcesResultType ImageClient::ListImageSources(
    const RPCParameters& parameters) {
    return ListImageSourcesAsync(parameters).get();
}

void ImageClient::OnListSourcesComplete(MessagePumpCallBase* call,
                                        const ::bosdyn::api::ListImageSourcesRequest& request,
                                        ::bosdyn::api::ListImageSourcesResponse&& response,
                                        const grpc::Status& status,
                                        std::promise<ImageListSourcesResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::ListImageSourcesResponse>(
        status, response, SDKErrorCode::Success);
    if (!ret_status) {
        promise.set_value({ret_status, std::move(response)});
        return;
    }

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<GetImageResultType> ImageClient::GetImageAsync(
    const std::vector<std::string>& image_sources,
    const RPCParameters& parameters) {
    std::promise<GetImageResultType> error_response;
    std::shared_future<GetImageResultType> error_future = error_response.get_future();

    ::bosdyn::api::GetImageRequest request;
    for (const std::string& source : image_sources) {
        ::bosdyn::api::ImageRequest image_request;
        BuildImageRequest(&image_request, source);
        ::bosdyn::api::ImageRequest* proto_image_request = request.add_image_requests();
        *proto_image_request = image_request;
    }

    return GetImageAsync(request, parameters);
}

GetImageResultType ImageClient::GetImage(
    const std::vector<std::string>& image_sources,
    const RPCParameters& parameters) {
    return GetImageAsync(image_sources, parameters).get();
}

std::shared_future<GetImageResultType> ImageClient::GetImageAsync(
    ::bosdyn::api::GetImageRequest& request, const RPCParameters& parameters) {
    std::promise<GetImageResultType> response;
    std::shared_future<GetImageResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time = InitiateAsyncCall<::bosdyn::api::GetImageRequest, ::bosdyn::api::GetImageResponse,
                                                      ::bosdyn::api::GetImageResponse>(
        request, std::bind(&::bosdyn::api::ImageService::Stub::AsyncGetImage, m_stub.get(), _1, _2, _3),
        std::bind(&ImageClient::OnGetImageComplete, this, _1, _2, _3, _4, _5), std::move(response),
        parameters);

    return future;
}

GetImageResultType ImageClient::GetImage(
    ::bosdyn::api::GetImageRequest& request, const RPCParameters& parameters) {
    return GetImageAsync(request, parameters).get();
}

void ImageClient::OnGetImageComplete(MessagePumpCallBase* call, const ::bosdyn::api::GetImageRequest& request,
                                     ::bosdyn::api::GetImageResponse&& response, const grpc::Status& status,
                                     std::promise<GetImageResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::GetImageResponse>(
        status, response, SDKErrorCode::Success);
    if (!ret_status) {
        promise.set_value({ret_status, std::move(response)});
        return;
    }

    // Iterate through the ImageResponse.status to verify there are no errors.
    for (const auto& image_response : response.image_responses()) {
        std::error_code code = image_response.status();
        if (code != SuccessCondition::Success) {
            promise.set_value({::bosdyn::common::Status(code, "ImageResponse ::bosdyn::common::Status unsuccessful"),
                               std::move(response)});
            return;
        }
    }

    promise.set_value({ret_status, std::move(response)});
}

void ImageClient::BuildImageRequest(::bosdyn::api::ImageRequest* request,
                                    const std::string& image_source_name,
                                    const double quality_percent,
                                    const ::bosdyn::api::Image_Format& image_format,
                                    const double resize_ratio) {
    request->set_image_source_name(image_source_name);
    request->set_quality_percent(quality_percent);
    request->set_image_format(image_format);
    request->set_resize_ratio(resize_ratio);
}

ServiceClient::QualityOfService ImageClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void ImageClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::ImageService::Stub(channel));
}

}  // namespace client

}  // namespace bosdyn
