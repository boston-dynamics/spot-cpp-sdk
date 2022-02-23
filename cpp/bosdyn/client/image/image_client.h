/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/image_service.grpc.pb.h>
#include <bosdyn/api/image_service.pb.h>

#include <future>
#include <memory>
#include <string>
#include <vector>

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/client/service_client/rpc_parameters.h"
#include "bosdyn/client/image/image_error_codes.h"

namespace bosdyn {

namespace client {

typedef Result<::bosdyn::api::ListImageSourcesResponse> ImageListSourcesResultType;
typedef Result<::bosdyn::api::GetImageResponse> GetImageResultType;

class ImageClient : public ServiceClient {
 public:
    ImageClient() { m_RPC_parameters.logging_control = LogRequestMode::kEnabled; }

    ~ImageClient() = default;

    // Asynchronous method to get a ListImageSourcesResponse.
    std::shared_future<ImageListSourcesResultType> ListImageSourcesAsync(
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to get a ListImageSourcesResponse.
    ImageListSourcesResultType ListImageSources(
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to get a GetImageResponse for a list of image sources. Defaults for
    // image quality and image type are used.
    std::shared_future<GetImageResultType> GetImageAsync(
        const std::vector<std::string>& image_sources,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to get a GetImageResponse for a list of image sources. Defaults for
    // image quality and image type are used.
    GetImageResultType GetImage(
        const std::vector<std::string>& image_sources,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to get a GetImageResponse for the given GetImageRequest.
    // request is not const because the method updates the header.
    std::shared_future<GetImageResultType> GetImageAsync(
        ::bosdyn::api::GetImageRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to get a GetImageResponse for the given GetImageRequest.
    // request is not const because the method updates the header.
    GetImageResultType GetImage(
        ::bosdyn::api::GetImageRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the Image service will be registered in the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the image service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous ListSources calls.
    void OnListSourcesComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::ListImageSourcesRequest& request,
        ::bosdyn::api::ListImageSourcesResponse&& response, const grpc::Status& status,
        std::promise<ImageListSourcesResultType> promise);

    // Callback function registered for the asynchronous GetImage calls.
    void OnGetImageComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::GetImageRequest& request,
        ::bosdyn::api::GetImageResponse&& response, const grpc::Status& status,
        std::promise<GetImageResultType> promise);

    static void BuildImageRequest(::bosdyn::api::ImageRequest* image_request, const std::string& image_source_name,
                           double quality_percent = 75.0,
                           const ::bosdyn::api::Image_Format& image_format = ::bosdyn::api::Image_Format_FORMAT_UNKNOWN,
                           const double resize_ratio = 0.0);

    std::unique_ptr<::bosdyn::api::ImageService::Stub> m_stub;

    // Default service name for the Image service.
    static const char* s_default_service_name;

    // Default service type for the image service.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
