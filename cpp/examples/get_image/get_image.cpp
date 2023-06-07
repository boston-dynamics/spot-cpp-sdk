/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include <CLI/CLI.hpp>

#include "bosdyn/client/sdk/client_sdk.h"
#include "bosdyn/client/image/image_client.h"
#include "bosdyn/client/util/cli_util.h"

int main(int argc, char** argv) {

    // Parse the arguments.
    CLI::App parser{"GetImage"};
    ::bosdyn::client::CommonCLIArgs common_args;
    ::bosdyn::client::AddCommonArguments(parser, common_args);
    CLI11_PARSE(parser, argc, argv);

    // Create a Client SDK object.
    std::unique_ptr<::bosdyn::client::ClientSdk> client_sdk = ::bosdyn::client::CreateStandardSDK(
        "get_image");
    std::cout << "------Created SDK" << std::endl;

    // Create a robot instance. A robot instance represents a single user on a single robot.
    ::bosdyn::client::Result<std::unique_ptr<::bosdyn::client::Robot>> robot_result =
        client_sdk->CreateRobot(common_args.hostname);
    if (!robot_result.status) {
        std::cerr << "Could not create robot: " << robot_result.status.DebugString() << std::endl;
        return 0;
    }
    std::unique_ptr<::bosdyn::client::Robot> robot = std::move(robot_result.response);
    std::cout << "------Created Robot" << std::endl;

    ::bosdyn::common::Status status = robot->Authenticate(common_args.username,
                                                          common_args.password);
    if (!status) {
        std::cerr << "Could not authenticate with robot: " << status.DebugString() << std::endl;
        return 0;
    }
    std::cout << "------Authenticated with Robot" << std::endl;

    // Create an ImageClient.
    ::bosdyn::client::Result<::bosdyn::client::ImageClient*> image_client_result =
        robot->EnsureServiceClient<::bosdyn::client::ImageClient>(
            ::bosdyn::client::ImageClient::GetDefaultServiceName());
    if (!image_client_result.status) {
        std::cerr << "Could not create image client: " << image_client_result.status.DebugString()
                  << std::endl;
        return 0;
    }

    // Asynchronously retrieve images in various ways.
    std::shared_future<::bosdyn::client::GetImageResultType> future_back =
        image_client_result.response->GetImageAsync({"back_fisheye_image"});
    std::shared_future<::bosdyn::client::GetImageResultType> future_fronts =
        image_client_result.response->GetImageAsync(
            {"frontleft_fisheye_image", "frontright_fisheye_image"});

    // Call API that accepts a full request
    ::bosdyn::api::GetImageRequest request_message;
    bosdyn::api::ImageRequest* image_request = request_message.add_image_requests();
    image_request->set_image_source_name("right_fisheye_image");
    image_request->set_quality_percent(65.1);
    image_request->set_image_format(bosdyn::api::Image_Format_FORMAT_JPEG);

    image_request = request_message.add_image_requests();
    image_request->set_image_source_name("left_fisheye_image");
    image_request->set_quality_percent(100.0);
    image_request->set_image_format(bosdyn::api::Image_Format_FORMAT_RAW);
    std::shared_future<::bosdyn::client::GetImageResultType> future_sides_full_message =
        image_client_result.response->GetImageAsync(request_message);

    // Make a different call before blocking on the futures.
    ::bosdyn::client::ImageListSourcesResultType result = image_client_result.response->ListImageSources();
    if (!result.status) {
        std::cerr << "ImageClient Error: " << result.status.DebugString() << std::endl;
        return 0;
    }
    std::cout << "------Image Sources: " << result.response.DebugString() << std::endl;

    // Now get the images from the async calls.
    ::bosdyn::client::GetImageResultType fut_result = future_back.get();
    if (!fut_result.status) {
        std::cerr << "Could not get back image " << fut_result.status.DebugString() << std::endl;
        return 0;
    }
    std::cout << "-----BACK IMAGE---:\n" << fut_result.response.DebugString() << std::endl;

    fut_result = future_sides_full_message.get();
    if (!fut_result.status) {
        std::cerr << "Could not get side images " << fut_result.status.DebugString() << std::endl;
        return 0;
    }
    std::cout << "------SIDE IMAGES---:\n" << fut_result.response.DebugString() << std::endl;

    fut_result = future_fronts.get();
    if (!fut_result.status) {
        std::cerr << "Could not get front images " << fut_result.status.DebugString() << std::endl;
        return 0;
    }
    std::cout << "------FRONT IMAGE---:\n" << fut_result.response.DebugString() << std::endl;

    return 0;
}
