/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


/** The inverse_kinematics_reachability example will show how to create a robot object, get the
 * E-Stop, timesync endpoint and lease associated with the robot, and then issue commands through
 * the robot command service and make requests through the inverse kinematics service.
 */
#define _USE_MATH_DEFINES
#include <cmath>

#include <bosdyn/api/robot_command.pb.h>
#include <bosdyn/api/robot_state.pb.h>
#include <bosdyn/api/spot/inverse_kinematics.pb.h>
#include <CLI/CLI.hpp>
#include <Eigen/Dense>
#include "bosdyn/client/inverse_kinematics/inverse_kinematics_client.h"
#include "bosdyn/client/lease/lease_client.h"
#include "bosdyn/client/robot_command/robot_command_builder.h"
#include "bosdyn/client/robot_command/robot_command_client.h"
#include "bosdyn/client/robot_command/robot_command_helpers.h"
#include "bosdyn/client/robot_state/robot_state_client.h"
#include "bosdyn/client/sdk/client_sdk.h"
#include "bosdyn/client/util/cli_util.h"
#include "bosdyn/math/api_common_frames.h"
#include "bosdyn/math/frame_helpers.h"
#include "bosdyn/math/proto_math.h"

int Sum(const std::vector<bool>& vec) { return std::accumulate(vec.begin(), vec.end(), 0); }

void PrintResults(const std::vector<bool>& reachable_ik, const std::vector<bool>& reachable_cmd) {
    // Classify the IK service reachability results as true positives, true negatives, false
    // positives, or false negatives.
    std::vector<bool> true_positives;
    std::vector<bool> true_negatives;
    std::vector<bool> false_positives;
    std::vector<bool> false_negatives;

    for (int i = 0; i < reachable_ik.size(); ++i) {
        true_positives.push_back(reachable_ik[i] && reachable_cmd[i]);
        true_negatives.push_back(!reachable_ik[i] && !reachable_cmd[i]);
        false_positives.push_back(reachable_ik[i] && !reachable_cmd[i]);
        false_negatives.push_back(!reachable_ik[i] && reachable_cmd[i]);
    }

    int total_true_positives = Sum(true_positives);
    int total_true_negatives = Sum(true_negatives);
    int total_false_positives = Sum(false_positives);
    int total_false_negatives = Sum(false_negatives);
    std::cout << "Total true positives: " << total_true_positives << std::endl;
    std::cout << "Total true negatives: " << total_true_negatives << std::endl;
    std::cout << "Total false positives: " << total_false_positives << std::endl;
    std::cout << "Total false negatives: " << total_false_negatives << std::endl;

    // Compute the false positive and negative rates.
    if ((total_false_positives + total_true_negatives) != 0) {
        double false_positive_rate =
            double(total_false_positives) / (total_false_positives + total_true_negatives);
        std::cout << "False positive rate: " << false_positive_rate << std::endl;
    }

    if ((total_false_negatives + total_true_positives) != 0) {
        double false_negative_rate =
            double(total_false_negatives) / (total_false_negatives + total_true_positives);
        std::cout << "False negative rate: " << false_negative_rate << std::endl;
    }
}

::bosdyn::common::Status run(const ::bosdyn::client::CommonCLIArgs& args,
                             const int num_poses) {  // Parse the arguments.
    // Create a Client SDK object.
    std::unique_ptr<::bosdyn::client::ClientSdk> client_sdk =
        ::bosdyn::client::CreateStandardSDK("inverse_kinematics_reachability");

    // Create a robot instance. A robot instance represents a single user on a single robot.
    ::bosdyn::client::Result<std::unique_ptr<::bosdyn::client::Robot>> robot_result =
        client_sdk->CreateRobot(args.hostname);
    if (!robot_result) {
        return robot_result.status.Chain("Could not create robot");
    }
    std::unique_ptr<::bosdyn::client::Robot> robot = std::move(robot_result.response);

    // Set the username and password to use for authentication with the robot.
    auto status = robot->Authenticate(args.username, args.password);
    if (!status) {
        return status.Chain("Could not authenticate with robot");
    }
    std::cout << "------Robot instance configured" << std::endl;

    // Create the robot state client.
    ::bosdyn::client::Result<::bosdyn::client::RobotStateClient*> robot_state_client_resp =
        robot->EnsureServiceClient<::bosdyn::client::RobotStateClient>();
    if (!robot_state_client_resp) {
        return robot_state_client_resp.status.Chain("Could not create the robot state client");
    }
    ::bosdyn::client::RobotStateClient* robot_state_client = robot_state_client_resp.response;

    // Make sure the robot has an arm.
    if (!robot_state_client->GetRobotState().response.robot_state().has_manipulator_state()) {
        return {::bosdyn::client::SDKErrorCode::GenericSDKError,
                "Robot must have an arm to run this example"};
    }

    // Create the robot command client.
    ::bosdyn::client::Result<::bosdyn::client::RobotCommandClient*> robot_cmd_client_resp =
        robot->EnsureServiceClient<::bosdyn::client::RobotCommandClient>();
    if (!robot_cmd_client_resp) {
        return robot_cmd_client_resp.status.Chain("Could not create the robot command client");
    }
    ::bosdyn::client::RobotCommandClient* robot_command_client = robot_cmd_client_resp.response;

    // Create the Inverse Kinematics client.
    ::bosdyn::client::Result<::bosdyn::client::InverseKinematicsClient*> ik_client_resp =
        robot->EnsureServiceClient<::bosdyn::client::InverseKinematicsClient>();
    if (!ik_client_resp) {
        return ik_client_resp.status.Chain("Could not create the inverse kinematics client");
    }
    ::bosdyn::client::InverseKinematicsClient* ik_client = ik_client_resp.response;

    // Use the Robot object's timesync endpoint and set a pointer within the robot command client
    // such that it can automatically update command end times in the robot's clock time.
    auto timesync_endpoint_res = robot->StartTimeSyncAndGetEndpoint();
    if (!timesync_endpoint_res) {
        return timesync_endpoint_res.status.Chain("Could not get the robot's timesync endpoint");
    }
    robot_command_client->AddTimeSyncEndpoint(timesync_endpoint_res.response);
    std::cout << "------TimeSync configured" << std::endl;

    // Now acquire a lease. First create a lease client.
    ::bosdyn::client::Result<::bosdyn::client::LeaseClient*> lease_client_resp =
        robot->EnsureServiceClient<::bosdyn::client::LeaseClient>();
    if (!lease_client_resp) {
        return lease_client_resp.status.Chain("Could not create the lease client");
    }
    ::bosdyn::client::LeaseClient* lease_client = lease_client_resp.response;
    // Then acquire the lease for the body.
    auto lease_res = lease_client->AcquireLease(::bosdyn::client::kBodyResource);
    if (!lease_res) {
        return lease_res.status.Chain("Could not acquire the body lease");
    }
    std::cout << "------Lease acquired" << std::endl;

    // Verify the robot is not estopped and that an external application has registered and holds
    // an estop endpoint.
    auto estop_status = robot->IsEstopped();
    if (!estop_status) {
        return estop_status.status.Chain("Could not check estop status");
    }
    if (estop_status.response) {
        return {::bosdyn::client::SDKErrorCode::GenericSDKError,
                "Robot is estopped. Please use an external E-Stop client, such as the estop "
                "Python SDK example, to configure E-Stop."};
    }
    std::cout << "------E-Stop configured" << std::endl;

    // Power on the robot.
    auto power_status = robot->PowerOnMotors(std::chrono::seconds(60), 1.0);
    if (!power_status) {
        return power_status.Chain("Could not power on the robot");
    }
    std::cout << "------Robot has powered on." << std::endl;

    // Tell the robot to stand up
    bosdyn::api::RobotCommand stand_command = ::bosdyn::client::StandCommand();
    auto stand_res = robot_command_client->RobotCommand(stand_command);
    if (!stand_res) {
        return stand_res.status.Chain("Failed to complete the stand command");
    }
    std::cout << "------Robot is standing." << std::endl;
    // Wait a few seconds before sending another command.
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Define a stand command that we'll send if the IK service does not find a solution.
    ::bosdyn::api::spot::MobilityParams mobility_params;
    mobility_params.mutable_body_control()
        ->mutable_body_assist_for_manipulation()
        ->set_enable_hip_height_assist(true);
    mobility_params.mutable_body_control()
        ->mutable_body_assist_for_manipulation()
        ->set_enable_body_yaw_assist(true);
    bosdyn::api::RobotCommand body_assist_enabled_stand_command =
        ::bosdyn::client::StandCommand(mobility_params);

    // Define the task frame to be in front of the robot and near the ground.
    auto robot_state = robot_state_client->GetRobotState().response.robot_state();
    ::bosdyn::api::SE3Pose odom_T_grav_body;
    bool success = ::bosdyn::api::get_a_tform_b(
        robot_state.kinematic_state().transforms_snapshot(), ::bosdyn::api::kOdomFrame,
        ::bosdyn::api::kGravAlignedBodyFrame, &odom_T_grav_body);
    if (!success) {
        return {::bosdyn::client::SDKErrorCode::GenericSDKError,
                "Failed to get a transform between the odom frame and the flat body frame."};
    }

    ::bosdyn::api::SE3Pose odom_T_gpe;
    success = ::bosdyn::api::get_a_tform_b(robot_state.kinematic_state().transforms_snapshot(),
                                           ::bosdyn::api::kOdomFrame,
                                           ::bosdyn::api::kGroundPlaneEstimateFrame, &odom_T_gpe);
    if (!success) {
        return {::bosdyn::client::SDKErrorCode::GenericSDKError,
                "Failed to get a transform between the odom frame and the ground plane "
                "estimate frame."};
    }
    // Construct the frame on the ground right underneath the center of the body.
    ::bosdyn::api::SE3Pose odom_T_ground_body = odom_T_grav_body;
    odom_T_ground_body.mutable_position()->set_z(odom_T_gpe.position().z());

    // Now, construct a task frame slightly above the ground, in front of the robot.
    ::bosdyn::api::SE3Pose odom_T_task =
        odom_T_ground_body *
        ::bosdyn::api::CreateSE3Pose(::bosdyn::api::CreateQuaternion(1, 0, 0, 0),
                                     ::bosdyn::api::CreateVec3(0.4, 0, 0.05));

    // Now, let's set our tool frame to be the tip of the robot's bottom jaw. Flip the orientation
    // so that when the hand is pointed downwards, the tool's z-axis is pointed upward.
    ::bosdyn::api::SE3Pose wr1_T_tool = ::bosdyn::api::CreateSE3Pose(
        ::bosdyn::api::FromPitch(-M_PI / 2), ::bosdyn::api::CreateVec3(0.23589, 0, -0.03943));

    // Populate the foot positions relative to the task frame for plotting later.
    ::bosdyn::api::SE3Pose odom_T_body;
    success = ::bosdyn::api::get_a_tform_b(robot_state.kinematic_state().transforms_snapshot(),
                                           ::bosdyn::api::kOdomFrame, ::bosdyn::api::kBodyFrame,
                                           &odom_T_body);
    if (!success) {
        return {::bosdyn::client::SDKErrorCode::GenericSDKError,
                "Failed to get a transform between the odom frame and the body frame."};
    }
    auto task_T_body = ~odom_T_task * odom_T_body;

    std::vector<std::pair<double, double>> foot_coords;
    for (int foot_index : {0, 1, 3, 2}) {
        auto foot_state = robot_state.foot_state()[foot_index];
        auto foot_position_rt_task = task_T_body * foot_state.foot_position_rt_body();
        foot_coords.push_back({foot_position_rt_task.x(), foot_position_rt_task.y()});
    }

    std::cout << "\nCommanding the arm to unstow." << std::endl;
    auto ready_command_res = ::bosdyn::client::ArmReadyCommand(body_assist_enabled_stand_command);
    if (!ready_command_res) {
        return ready_command_res.status.Chain("Failed to construct the arm ready command");
    }
    auto unstow_res = robot_command_client->RobotCommand(ready_command_res.response);
    if (!unstow_res) {
        return unstow_res.status.Chain("Failed to send the arm ready command");
    }
    constexpr int move_timeout_sec = 5;
    auto block_res = BlockUntilArmArrives(*robot_command_client,
                                          unstow_res.response.robot_command_id(), move_timeout_sec);
    if (!block_res) {
        return block_res.status.Chain("Ready command failed");
    }
    std::cout << "------Unstow complete." << std::endl;

    // The desired tool poses are defined relative to a task frame in front of the robot and
    // slightly above the ground. The task frame is aligned with the "gravity aligned body
    // frame", such that the positive-x direction is to the front of the robot, the positive-y
    // direction is to the left of the robot, and the positive-z direction is opposite to
    // gravity.
    double x_size = 0.7;  // m
    double y_size = 0.8;  // m
    Eigen::VectorXd x_rt_task = Eigen::VectorXd::Random(num_poses);
    x_rt_task = (x_rt_task + Eigen::VectorXd::Constant(num_poses, 1.0)) * (x_size / 2);
    Eigen::VectorXd y_rt_task = (-y_size / 2) * Eigen::VectorXd::Random(num_poses);
    std::vector<::bosdyn::api::SE3Pose> task_T_desired_tools;
    for (int i = 0; i < num_poses; ++i) {
        task_T_desired_tools.push_back(::bosdyn::api::CreateSE3Pose(
            ::bosdyn::api::CreateQuaternion(1, 0, 0, 0),
            ::bosdyn::api::CreateVec3(x_rt_task[i], y_rt_task[i], 0.0)));
    }

    // These arrays store the reachability results as determined by the IK responses
    // (`reachable_ik`) or by trying to move to the desired tool pose (`reachable_cmd`).
    std::vector<bool> reachable_ik;
    std::vector<bool> reachable_cmd;
    for (int i = 0; i < task_T_desired_tools.size(); ++i) {
        // Query the IK service for the reachability of the desired tool pose.
        // Construct the IK request for this reachability problem. Note that since
        // `root_tform_scene` is unset, the "scene" frame is the same as the "root" frame in
        // this case.

        auto task_T_desired_tool = task_T_desired_tools[i];
        ::bosdyn::api::spot::InverseKinematicsRequest ik_request;
        ik_request.set_root_frame_name(::bosdyn::api::kOdomFrame);
        *(ik_request.mutable_scene_tform_task()) = odom_T_task;
        *(ik_request.mutable_wrist_mounted_tool()->mutable_wrist_tform_tool()) = wr1_T_tool;
        *(ik_request.mutable_tool_pose_task()->mutable_task_tform_desired_tool()) =
            task_T_desired_tool;

        // Note: we're not checking the return status of this request, because we expect some of
        // the requests to fail. We'll add the results to a list and count the numbers of
        // failures later.
        ::bosdyn::client::InverseKinematicsResultType ik_result =
            ik_client->InverseKinematics(ik_request);

        auto ik_response = ik_result.response;
        reachable_ik.push_back(ik_response.status() ==
                               ::bosdyn::api::spot::InverseKinematicsResponse::STATUS_OK);

        // Attempt to move to each of the desired tool poses to check the IK results.
        if (ik_response.status() == ::bosdyn::api::spot::InverseKinematicsResponse::STATUS_OK) {
            ::bosdyn::api::SE3Pose odom_T_desired_body;
            success = get_a_tform_b(ik_response.robot_configuration().transforms_snapshot(),
                                    ::bosdyn::api::kOdomFrame, ::bosdyn::api::kBodyFrame,
                                    &odom_T_desired_body);
            if (!success) {
                return {::bosdyn::client::SDKErrorCode::GenericSDKError,
                        "Failed to get a desired transform between the odom frame and the "
                        "body frame."};
            }

            ::bosdyn::api::spot::MobilityParams mobility_params;
            *(mobility_params.mutable_body_control()->mutable_body_pose()) =
                ::bosdyn::client::BodyPose(::bosdyn::api::kOdomFrame, odom_T_desired_body);
            stand_command = ::bosdyn::client::StandCommand(mobility_params);
        } else if (ik_response.status() ==
                       ::bosdyn::api::spot::InverseKinematicsResponse::STATUS_UNKNOWN ||
                   ik_response.status() ==
                       ::bosdyn::api::spot::InverseKinematicsResponse::STATUS_NO_SOLUTION_FOUND) {
            stand_command = body_assist_enabled_stand_command;
        }

        auto arm_command_res = ::bosdyn::client::ArmPoseCommandFromPose(
            stand_command, odom_T_task * task_T_desired_tool, ::bosdyn::api::kOdomFrame, 1);
        if (!arm_command_res) {
            return arm_command_res.status.Chain("Failed to construct the arm pose command");
        }
        arm_command_res.response.mutable_synchronized_command()
            ->mutable_arm_command()
            ->mutable_arm_cartesian_command()
            ->mutable_wrist_tform_tool()
            ->CopyFrom(wr1_T_tool);
        auto arm_pose_command_res = robot_command_client->RobotCommand(arm_command_res.response);
        if (!arm_pose_command_res) {
            return arm_pose_command_res.status.Chain("Failed to execute the arm pose command");
        }
        block_res = BlockUntilArmArrives(*robot_command_client,
                                         arm_pose_command_res.response.robot_command_id(), 2);
        reachable_cmd.push_back(block_res);
    }

    PrintResults(reachable_ik, reachable_cmd);
    // Sit down the robot.
    bosdyn::api::RobotCommand poweroff_command = ::bosdyn::client::SafePowerOffCommand();
    auto poweroff_res = robot_command_client->RobotCommand(poweroff_command);
    if (!poweroff_res) {
        return poweroff_res.status.Chain("Failed to complete the safe power off command");
    }
    std::cout << "------Robot is powered off." << std::endl;

    return {::bosdyn::client::SDKErrorCode::Success};
}

int main(int argc, char** argv) {
    // Parse the arguments.
    CLI::App parser{"InverseKinematicsReachability"};
    ::bosdyn::client::CommonCLIArgs common_args;
    ::bosdyn::client::AddCommonArguments(parser, common_args);

    int num_poses;
    parser.add_option("--num_poses", num_poses, "Number of desired tool poses to query.")
        ->default_val(50);
    CLI11_PARSE(parser, argc, argv);

    auto status = run(common_args, num_poses);
    if (!status) {
        std::cerr << status.DebugString() << std::endl;
        return 1;
    }

    std::cout << "------InverseKinematicsReachability application worked correctly." << std::endl;
    return 0;
}
