/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include <google/protobuf/util/time_util.h>
#include <CLI/CLI.hpp>

#include "bosdyn/client/lease/lease_keepalive.h"
#include "bosdyn/client/robot_command/robot_command_client.h"
#include "bosdyn/client/robot_command/robot_command_helpers.h"
#include "bosdyn/client/sdk/client_sdk.h"
#include "bosdyn/client/service_client/client_header_handling.h"
#include "bosdyn/client/time_sync/time_sync_helpers.h"
#include "bosdyn/client/util/cli_util.h"
#include "examples/joint_control/constants.hpp"
#include "examples/joint_control/joint_api_helper.hpp"

#include <unistd.h>
#include <chrono>
#include <cmath>
#include <thread>

::bosdyn::common::Status AcquireLease(::bosdyn::client::Robot* robot) {
    // Now acquire a lease. First create a lease client.
    ::bosdyn::client::Result<::bosdyn::client::LeaseClient*> lease_client_resp =
        robot->EnsureServiceClient<::bosdyn::client::LeaseClient>();
    if (!lease_client_resp) {
        return lease_client_resp.status.Chain("Could not create the lease client");
    }
    ::bosdyn::client::LeaseClient* lease_client = lease_client_resp.response;
    // Then acquire the lease for the body.
    auto lease_res = lease_client->AcquireLease("body");
    if (!lease_res) {
        return lease_res.status.Chain("Could not acquire the body lease");
    }

    return {::bosdyn::client::SDKErrorCode::Success};
}

::bosdyn::common::Status EstopStatusCheck(::bosdyn::client::Robot* robot) {
    // Verify the robot is not estopped and that an external application has registered and holds
    // an estop endpoint.
    auto estop_status = robot->IsEstopped();
    if (!estop_status) {
        return estop_status.status.Chain("Could not check estop status");
    }
    if (estop_status.response) {
        std::cerr << "Robot is estopped. Please use an external E-Stop client, such as the estop"
                     "Python SDK example, to configure E-Stop."
                  << std::endl;
        return {::bosdyn::client::SDKErrorCode::GenericSDKError};
    }
    return {::bosdyn::client::SDKErrorCode::Success};
}

::bosdyn::common::Status PowerOnMotors(::bosdyn::client::Robot* robot) {
    // Power on the robot.
    std::cout << "------Powering On" << std::endl;
    auto power_status = robot->PowerOnMotors(std::chrono::seconds(60), 1.0);
    if (!power_status) {
        std::cerr << "Could not power on the robot: " << power_status.DebugString() << std::endl;
        return power_status;
    }
    return {::bosdyn::client::SDKErrorCode::Success};
}

::bosdyn::common::Status run(const ::bosdyn::client::CommonCLIArgs& args) {
    /**
     * An example of using joint control to command a Spot robot through the Boston Dynamics API.
     * This example will,
     * 1. Power on the robot
     * 2. Stand the robot up
     * 3. Start streaming joint control state messages
     * 4. Start streaming joint control command messages
     *      a. Move WR0 and gripper (F1X)
     *      b. Lock all other joints in their initial position, including using the initial torque
     *         as a feed forward torque to mimic gravity compensation.
     * 5. Activate joint control
     * 6. Sit down and power off the robot
     */

    // Create a Client SDK object.
    std::unique_ptr<::bosdyn::client::ClientSdk> client_sdk =
        ::bosdyn::client::CreateStandardSDK("JointControlClient");
    std::cout << "------Created SDK" << std::endl;

    // Create a robot instance. A robot instance represents a single user on a single robot.
    auto robot_result = client_sdk->CreateRobot(args.hostname);
    if (!robot_result) {
        return robot_result.status.Chain("Could not create robot");
    }
    auto robot = robot_result.move();
    std::cout << "------Created Robot" << std::endl;

    // Set the username and password to use. If you do not already have an account on a robot, ask
    // your administrator to add an account.
    ::bosdyn::common::Status status = robot->Authenticate(args.username, args.password);
    if (!status) {
        return status.Chain("Could not authenticate with robot");
    }
    std::cout << "------Authenticated with Robot" << std::endl;

    // Establish time synchronization with the robot
    ::bosdyn::client::Result<::bosdyn::client::TimeSyncClient*> time_sync_client_resp =
        robot->EnsureServiceClient<::bosdyn::client::TimeSyncClient>();
    if (!time_sync_client_resp.status) {
        std::cerr << "Could not create the time sync client: "
                  << time_sync_client_resp.status.DebugString() << std::endl;
        return time_sync_client_resp.status;
    }
    ::bosdyn::client::TimeSyncClient* time_sync_client = time_sync_client_resp.response;
    ::bosdyn::client::TimeSyncThread time_sync_thread(time_sync_client);
    if (time_sync_thread.HasEstablishedTimeSync()) {
        std::cerr << "Faulty establishment of time sync." << std::endl;
        return {::bosdyn::client::SDKErrorCode::GenericSDKError};
    }
    // Start time sync
    time_sync_thread.Start();
    if (!time_sync_thread.WaitForSync(std::chrono::seconds(5))) {
        std::cerr << "Failed to establish time sync before timing out." << std::endl;
        return {::bosdyn::client::SDKErrorCode::GenericSDKError};
    }
    std::cout << "------TimeSync Complete" << std::endl;

    // Estop check
    status = EstopStatusCheck(robot.get());
    if (!status) {
        return status;
    }
    std::cout << "------E-Stop Configured" << std::endl;

    // Lease acquire
    status = AcquireLease(robot.get());
    if (!status) {
        return status;
    }
    std::cout << "------Lease Acquired" << std::endl;

    // Power on motors
    status = PowerOnMotors(robot.get());
    if (!status) {
        return status;
    }
    std::cout << "------Robot has powered on." << std::endl;

    // Create the robot command client.
    ::bosdyn::client::Result<::bosdyn::client::RobotCommandClient*> robot_cmd_client_resp =
        robot->EnsureServiceClient<::bosdyn::client::RobotCommandClient>();
    if (!robot_cmd_client_resp) {
        return robot_cmd_client_resp.status.Chain("Could not create the robot command client");
    }
    ::bosdyn::client::RobotCommandClient* robot_command_client = robot_cmd_client_resp.response;
    robot_command_client->AddTimeSyncEndpoint(time_sync_thread.GetEndpoint());
    std::cout << "-----Get RobotCommandClient Complete" << std::endl;

    // Stand up the robot.
    bosdyn::api::RobotCommand stand_command = ::bosdyn::client::StandCommand();
    auto stand_res = robot_command_client->RobotCommand(stand_command);
    if (!stand_res.status) {
        std::cerr << "Failed to complete the stand command: " << stand_res.status.DebugString()
                  << std::endl;
        return stand_res.status;
    }
    auto block_res =
        BlockUntilStandComplete(*robot_command_client, stand_res.response.robot_command_id());
    if (!block_res) {
        return block_res.status.Chain("Stand command failed");
    }

    // Extra delay to make sure
    std::this_thread::sleep_for(std::chrono::seconds(3));

    JointAPIInterface<SpotKin::N_DOF> joint_api_interface;

    // Start state streaming
    std::cout << "------Start state stream" << std::endl;
    auto robot_state_stream_client_resp =
        robot->EnsureServiceClient<::bosdyn::client::RobotStateStreamingClient>();
    if (!robot_state_stream_client_resp) {
        return robot_cmd_client_resp.status.Chain(
            "Could not create the robot state stream client.");
    }
    auto robot_state_stream_client = robot_state_stream_client_resp.move();
    std::thread state_thread(&JointAPIInterface<SpotKin::N_DOF>::handle_state_streaming,
                             &joint_api_interface, robot_state_stream_client);

    // Async activate once streaming has started
    std::thread activate_thread(&JointAPIInterface<SpotKin::N_DOF>::activate, &joint_api_interface,
                                robot_command_client);

    // Once state streaming has started and before we take control, cache the joint state.
    auto joints_state = joint_api_interface.get_latest_joints_state();
    auto curr_pose = joints_state.pos;
    auto curr_load = joints_state.load;

    std::vector<std::array<float, SpotKin::N_DOF>> cmd_poses;
    std::array<float, SpotKin::N_DOF> pos_des1, pos_des2;
    pos_des1 = pos_des2 = curr_pose;
    pos_des1[spot_kin.A0_WR0] = pos_des1[spot_kin.A0_WR0] + 1.0f;
    pos_des1[spot_kin.A0_F1X] = pos_des1[spot_kin.A0_F1X] - 0.5f;
    pos_des2[spot_kin.A0_WR0] = pos_des2[spot_kin.A0_WR0] - 1.0f;
    pos_des2[spot_kin.A0_F1X] = pos_des2[spot_kin.A0_F1X] - 0.5f;

    cmd_poses.push_back(curr_pose);
    cmd_poses.push_back(pos_des1);
    cmd_poses.push_back(curr_pose);
    cmd_poses.push_back(pos_des2);
    cmd_poses.push_back(curr_pose);
    cmd_poses.push_back(pos_des1);
    cmd_poses.push_back(curr_pose);
    cmd_poses.push_back(pos_des2);
    cmd_poses.push_back(curr_pose);

    // Stream commands to the robot by passing an iterator
    auto robot_command_stream_client_resp =
        robot->EnsureServiceClient<::bosdyn::client::RobotCommandStreamingClient>();

    if (!robot_command_stream_client_resp) {
        return robot_command_stream_client_resp.status.Chain(
            "Could not create the robot command stream client");
    }

    int64_t duration_bw_pose_ns = 1 * 1e9;
    auto joints_k_q_p = spot_kin.gains.k_q_p;
    auto joints_k_qd_p = spot_kin.gains.k_qd_p;

    std::cout << "------Start command stream" << std::endl;
    status = joint_api_interface.generate_joint_pos_interp_commands(
        robot_command_stream_client_resp.move(), &time_sync_thread, cmd_poses, curr_load,
        duration_bw_pose_ns, joints_k_q_p, joints_k_qd_p);
    if (!status) {
        return status;
    }
    std::cout << "------Sending command completed" << std::endl;

    joint_api_interface.set_should_stop(true);
    if (state_thread.joinable()) state_thread.join();
    if (activate_thread.joinable()) activate_thread.join();

    // Sit down the robot.
    bosdyn::api::RobotCommand poweroff_command = ::bosdyn::client::SafePowerOffCommand();
    auto poweroff_res = robot_command_client->RobotCommand(poweroff_command);
    if (!poweroff_res) {
        return poweroff_res.status.Chain("Failed to complete the safe power off command");
    }
    std::cout << "------Robot is powered off." << std::endl;

    time_sync_thread.Stop();

    return {::bosdyn::client::SDKErrorCode::Success};
}

int main(int argc, char** argv) {
    // Parse the arguments.
    CLI::App parser{"WiggleArmExample"};
    ::bosdyn::client::CommonCLIArgs common_args;
    ::bosdyn::client::AddCommonArguments(parser, common_args);
    CLI11_PARSE(parser, argc, argv);

    auto status = run(common_args);
    if (!status) {
        std::cerr << status.DebugString() << std::endl;
        return 1;
    }

    std::cout << "------WiggleArmExample application worked correctly." << std::endl;
    return 0;
}
