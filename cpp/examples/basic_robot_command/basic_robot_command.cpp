/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


/** The basic_robot_command example will show to create a robot object, get the E-Stop, timesync
 * endpoint and lease associated with the robot, and then issue commands through the robot
 * command service.
 *
 * Specifically, this robot will standup and then walk forward 1 meter using the robot
 * command's Go-To in body frame helper functions.
 *
 * To run this example, you must provide the username and password for the robot, as well as the
 * robot's hostname/IP address:
 *     ./basic_robot_command USERNAME PASSWORD ROBOT_IP
 */

#include <bosdyn/api/geometry.pb.h>
#include <CLI/CLI.hpp>
#include "bosdyn/client/sdk/client_sdk.h"
#include "bosdyn/client/lease/lease_client.h"
#include "bosdyn/math/frame_helpers.h"
#include "bosdyn/math/proto_math.h"
#include "bosdyn/client/robot_command/robot_command_builder.h"
#include "bosdyn/client/robot_command/robot_command_client.h"
#include "bosdyn/client/robot_id/robot_id_client.h"
#include "bosdyn/client/time_sync/time_sync_helpers.h"
#include "bosdyn/client/util/cli_util.h"


int main(int argc, char** argv) {

    // Parse the arguments.
    CLI::App parser{"BasicRobotCommand"};
    ::bosdyn::client::CommonCLIArgs common_args;
    ::bosdyn::client::AddCommonArguments(parser, common_args);
    CLI11_PARSE(parser, argc, argv);

    // Create a Client SDK object.
    std::unique_ptr<::bosdyn::client::ClientSdk> client_sdk = ::bosdyn::client::CreateStandardSDK("basic_cmd_spot");

    // Create a robot instance. A robot instance represents a single user on a single robot.
    ::bosdyn::client::Result<std::unique_ptr<::bosdyn::client::Robot>> robot_result =
        client_sdk->CreateRobot(common_args.hostname);
    if (!robot_result.status) {
        std::cerr << "Could not create robot: " << robot_result.status.DebugString() << std::endl;
        return 0;
    }
    std::unique_ptr<::bosdyn::client::Robot> robot = std::move(robot_result.response);

    // Set the username and password to use for authentication with the robot.
    auto status = robot->Authenticate(common_args.username, common_args.password);
    if (!status) {
        std::cerr << "Could not authenticate with robot: " << status.DebugString() << std::endl;
        return 0;
    }
    std::cout << "------Robot instance configured" << std::endl;

    // Create the robot command client.
    ::bosdyn::client::Result<::bosdyn::client::RobotCommandClient*> robot_cmd_client_resp =
        robot->EnsureServiceClient<::bosdyn::client::RobotCommandClient>(
            ::bosdyn::client::RobotCommandClient::GetDefaultServiceName());
    if (!robot_cmd_client_resp.status) {
        std::cerr << "Could not create the robot command client: "
                  << robot_cmd_client_resp.status.DebugString() << std::endl;
        return 0;
    }
    ::bosdyn::client::RobotCommandClient* robot_command_client =
        robot_cmd_client_resp.response;

    // Use the Robot object's timesync endpoint and set a pointer within the robot command client
    // such that it can automatically update command end times in the robot's clock time.
    auto timesync_endpoint_res = robot->StartTimeSyncAndGetEndpoint();
    if (!timesync_endpoint_res.status) {
        std::cerr << "Could not get the robot's timesync endpoint: " << timesync_endpoint_res.status
                  << std::endl;
        return 0;
    }
    robot_command_client->AddTimeSyncEndpoint(timesync_endpoint_res.response);
    std::cout << "------TimeSync configured" << std::endl;

    // Now acquire a lease. First create a lease client.
    ::bosdyn::client::Result<::bosdyn::client::LeaseClient*> lease_client_resp =
        robot->EnsureServiceClient<::bosdyn::client::LeaseClient>(
            ::bosdyn::client::LeaseClient::GetDefaultServiceName());
    if (!lease_client_resp.status) {
        std::cerr << "Could not create the lease client: " << lease_client_resp.status.DebugString()
                  << std::endl;
        return 0;
    }
    ::bosdyn::client::LeaseClient* lease_client = lease_client_resp.response;
    // Then acquire the lease for the body.
    auto lease_res = lease_client->AcquireLease("body");
    if (!lease_res.status) {
        std::cerr << "Could not acquire the body lease: " << lease_res.status.DebugString()
                  << std::endl;
        return 0;
    }
    std::cout << "------Lease acquired" << std::endl;

    // Verify the robot is not estopped and that an external application has registered and holds
    // an estop endpoint.
    auto estop_status = robot->IsEstopped();
    if (!estop_status) {
        std::cerr << "Could not check estop status: " << estop_status.status << std::endl;
        return 0;
    }
    if (estop_status.response) {
        std::cerr << "Robot is estopped. Please use an external E-Stop client, such as the estop "
                "Python SDK example, to configure E-Stop." << std::endl;
        return 0;
    }
    std::cout << "------E-Stop configured" << std::endl;

    // Power on the robot.
    auto power_status = robot->PowerOnMotors(std::chrono::seconds(60), 1.0);
    if (!power_status) {
        std::cerr << "Could not power on the robot: " << power_status.DebugString() << std::endl;
        return 0;
    }
    std::cout << "------Robot has powered on." << std::endl;

    // Stand up the robot.
    bosdyn::api::RobotCommand stand_command = ::bosdyn::client::StandCommand();
    auto stand_res = robot_command_client->RobotCommand(stand_command);
    if (!stand_res.status) {
        std::cerr << "Failed to complete the stand command: " << stand_res.status.DebugString()
                  << std::endl;
        return 0;
    }
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Get the current instance of the frame tree snapshot from the robot object (which will use the
    // robot state client).
    auto frame_tree_snapshot_res = robot->GetFrameTreeSnapshot();
    if (!frame_tree_snapshot_res.status) {
        std::cerr << "Could not get the frame tree snapshot: "
                  << frame_tree_snapshot_res.status.DebugString() << std::endl;
        return 0;
    }
    bosdyn::api::FrameTreeSnapshot frame_tree_snapshot = *frame_tree_snapshot_res.response;

    // Issue a trajectory command in the body frame. We will make the robot move forward 1 meter.
    auto command_res = ::bosdyn::client::TrajectoryCommandInBodyFrame(1.0, 0.0, 0.0, frame_tree_snapshot);
    if (!command_res.status) {
        std::cerr << "Failed to get a transform between the odom frame and the body frame: "
                  << command_res.status << std::endl;
        return 0;
    }
    auto goto_res = robot_command_client->RobotCommand(
        command_res.response, nullptr, nullptr,
        std::chrono::system_clock::now() + std::chrono::seconds(10));
    if (!goto_res.status) {
        std::cerr << "Failed to complete the goto command: " << goto_res.status.DebugString()
                  << std::endl;
        return 0;
    }

    // Stand up the robot.
    bosdyn::api::RobotCommand poweroff_command = ::bosdyn::client::SafePowerOffCommand();
    auto poweroff_res = robot_command_client->RobotCommand(poweroff_command);
    if (!poweroff_res.status) {
        std::cerr << "Failed to complete the safe power off command: " << poweroff_res.status.DebugString()
                  << std::endl;
        return 0;
    }
    std::cout << "------Robot is powered off." << std::endl;

    return 0;
}
