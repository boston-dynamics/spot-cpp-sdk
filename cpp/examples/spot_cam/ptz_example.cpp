/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */



#include <atomic>
#include <string>
#include <vector>
#include <CLI/CLI.hpp>

#include "bosdyn/client/fault/fault_client.h"
#include "bosdyn/client/fault/util.h"
#include "bosdyn/client/sdk/client_sdk.h"
#include "bosdyn/client/spot_cam/ptz/ptz_client.h"
#include "bosdyn/client/util/cli_util.h"


// Server which services GRPC requests for point clouds using the velodyne driver.
bosdyn::common::Status run(const ::bosdyn::client::CommonCLIArgs& args,
                           const std::string& ptz_choice, double pan, double tilt, double zoom) {

    // Create an sdk, robot, and payload registration client.
    std::unique_ptr<bosdyn::client::ClientSdk> client_sdk = bosdyn::client::CreateStandardSDK(
        "PTZ_command");
    std::cout << "------Created SDK" << std::endl;
    auto client_result = client_sdk->CreateRobot(args.hostname);
    if (!client_result) {
        return client_result.status.Chain("Unable to connect to robot");
    }
    std::unique_ptr<bosdyn::client::Robot> robot = client_result.move();

    auto status = robot->Authenticate(args.username, args.password);
    if ( !status) {
        return status;
    }
    std::cout << "------Robot instance configured" << std::endl;

    bosdyn::client::Result<bosdyn::client::DirectoryClient*> dir_client_result =
        robot->EnsureServiceClient<bosdyn::client::DirectoryClient>(
            bosdyn::client::DirectoryClient::GetDefaultServiceName());
    if (!dir_client_result) {
        return dir_client_result.status.Chain("Could not create directory client");
    }

    std::cout << "Waiting for PTZ service to register ..." << std::endl;
    while (true) {
        bosdyn::client::DirectoryEntryResultType fut_result =
            dir_client_result.response->GetServiceEntry(
                bosdyn::client::spot_cam::PtzClient::GetDefaultServiceName());

        if (fut_result) {
            std::cout << "------PTZ Service found:\n" << fut_result.response.DebugString() << std::endl;
            break;
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    auto ptz_result = robot->EnsureServiceClient<bosdyn::client::spot_cam::PtzClient>();
    if (!ptz_result) {
        return ptz_result.status.Chain("Could not create PTZ client");
    }
    auto ptz_client = ptz_result.move();

    auto setptz_result = ptz_client->SetPtzPosition(ptz_choice, pan, tilt, zoom);
    if (!setptz_result) return setptz_result.status.Chain("Could not set PTZ position");

    auto resp = ptz_client->GetPtzPosition(ptz_choice);
    if (!resp) {
        return resp.status;
    }

    std::cout << "------PTZ reported position:\n" << resp.response.DebugString() << std::endl;

    return {::bosdyn::client::SDKErrorCode::Success};
}

int main(int argc, char** argv) {
    // Parse the arguments.
    CLI::App parser{"HelloSpot"};
    ::bosdyn::client::CommonCLIArgs common_args;
    ::bosdyn::client::AddCommonArguments(parser, common_args);

    std::string ptz_choice = "digi";
    double pan = 0.0, tilt = 0.0, zoom = 0.0;
    parser.add_option("--ptz-choice", ptz_choice, "PTZ to set position of.")->required();
    parser.add_option("--pan", pan, "Pan position of the PTZ.")->required();
    parser.add_option("--tilt", tilt, "Tilt position of the PTZ.")->required();
    parser.add_option("--zoom", zoom, "Zoom position of the PTZ")->required();

    CLI11_PARSE(parser, argc, argv);

    auto status = run(common_args, ptz_choice, pan, tilt, zoom);
    if (!status) {
        std::cerr << status.DebugString() << std::endl;
        return 1;
    }
    return 0;
}
