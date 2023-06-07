/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "robot_command_builder.h"

#include <bosdyn/api/robot_command.pb.h>
#include <bosdyn/api/spot/robot_command.pb.h>

#include "bosdyn/client/error_codes/sdk_error_code.h"
#include "bosdyn/client/robot_state/robot_state_client.h"
#include "bosdyn/client/service_client/client_header_handling.h"
#include "bosdyn/client/service_client/result.h"
#include "bosdyn/math/frame_helpers.h"
#include "bosdyn/math/proto_math.h"

namespace bosdyn {

namespace client {

namespace {
constexpr double kClawGripperOpenAngle = -1.5708;
constexpr double kClawGripperClosedAngle = 0;
}  // namespace

::bosdyn::api::RobotCommand StopCommand() {
    ::bosdyn::api::RobotCommand command;
    command.mutable_full_body_command()->mutable_stop_request();
    return command;
}

::bosdyn::api::RobotCommand FreezeCommand() {
    ::bosdyn::api::RobotCommand command;
    command.mutable_full_body_command()->mutable_freeze_request();
    return command;
}

::bosdyn::api::RobotCommand SelfrightCommand() {
    ::bosdyn::api::RobotCommand command;
    command.mutable_full_body_command()->mutable_selfright_request();
    return command;
}

::bosdyn::api::RobotCommand SafePowerOffCommand() {
    ::bosdyn::api::RobotCommand command;
    command.mutable_full_body_command()->mutable_safe_power_off_request();
    return command;
}

::bosdyn::api::RobotCommand SitCommand(const ::bosdyn::api::spot::MobilityParams& params) {
    ::bosdyn::api::RobotCommand command;
    command.mutable_synchronized_command()->mutable_mobility_command()->mutable_params()->CopyFrom(
        ToAny(params));
    command.mutable_synchronized_command()->mutable_mobility_command()->mutable_sit_request();
    return command;
}

::bosdyn::api::RobotCommand SitCommand() {
    ::bosdyn::api::spot::MobilityParams empty_params;
    return SitCommand(empty_params);
}

::bosdyn::api::RobotCommand StandCommand(const ::bosdyn::api::spot::MobilityParams& params) {
    ::bosdyn::api::RobotCommand command;
    command.mutable_synchronized_command()->mutable_mobility_command()->mutable_params()->CopyFrom(
        ToAny(params));
    command.mutable_synchronized_command()->mutable_mobility_command()->mutable_stand_request();
    return command;
}

::bosdyn::api::RobotCommand StandCommand() {
    ::bosdyn::api::spot::MobilityParams empty_params;
    return StandCommand(empty_params);
}

::bosdyn::api::spot::BodyControlParams::BodyPose BodyPose(const std::string& frame_name,
                                                          const ::bosdyn::api::SE3Pose& body_pose) {
    ::bosdyn::api::spot::BodyControlParams::BodyPose pose;
    pose.set_root_frame_name(frame_name);
    *(pose.mutable_base_offset_rt_root()->add_points()->mutable_pose()) = body_pose;
    return pose;
}

::bosdyn::api::RobotCommand TrajectoryCommand(const ::bosdyn::api::SE2Pose& goto_pose,
                                              const std::string& se2_frame_name,
                                              const ::bosdyn::api::spot::MobilityParams& params) {
    ::bosdyn::api::RobotCommand command;
    command.mutable_synchronized_command()->mutable_mobility_command()->mutable_params()->CopyFrom(
        ToAny(params));
    AddGotoToCommand(goto_pose, se2_frame_name, &command);
    return command;
}

::bosdyn::api::RobotCommand TrajectoryCommand(const ::bosdyn::api::SE2Pose& goto_pose,
                                              const std::string& se2_frame_name) {
    ::bosdyn::api::spot::MobilityParams empty_params;
    return TrajectoryCommand(goto_pose, se2_frame_name, empty_params);
}

::bosdyn::api::RobotCommand TrajectoryCommand(const double& goal_x, const double& goal_y,
                                              const double& goal_heading,
                                              const std::string& se2_frame_name,
                                              const ::bosdyn::api::spot::MobilityParams& params) {
    ::bosdyn::api::RobotCommand command;
    command.mutable_synchronized_command()->mutable_mobility_command()->mutable_params()->CopyFrom(
        ToAny(params));
    ::bosdyn::api::SE2Pose goto_pose;
    goto_pose.mutable_position()->set_x(goal_x);
    goto_pose.mutable_position()->set_y(goal_y);
    goto_pose.set_angle(goal_heading);
    AddGotoToCommand(goto_pose, se2_frame_name, &command);
    return command;
}

::bosdyn::api::RobotCommand TrajectoryCommand(const double& goal_x, const double& goal_y,
                                              const double& goal_heading,
                                              const std::string& se2_frame_name) {
    ::bosdyn::api::spot::MobilityParams empty_params;
    return TrajectoryCommand(goal_x, goal_y, goal_heading, se2_frame_name, empty_params);
}

Result<::bosdyn::api::RobotCommand> TrajectoryCommandInBodyFrame(
    const double& goal_x_rt_body, const double& goal_y_rt_body, const double& goal_heading_rt_body,
    const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot,
    const ::bosdyn::api::spot::MobilityParams& params) {
    ::bosdyn::api::SE2Pose goto_rt_body;
    goto_rt_body.mutable_position()->set_x(goal_x_rt_body);
    goto_rt_body.mutable_position()->set_y(goal_y_rt_body);
    goto_rt_body.set_angle(goal_heading_rt_body);
    ::bosdyn::api::SE2Pose odom_tform_body;
    bool success =
        ::bosdyn::api::get_a_tform_b(frame_tree_snapshot, ::bosdyn::api::kOdomFrame,
                                     ::bosdyn::api::kGravAlignedBodyFrame, &odom_tform_body);
    if (!success) {
        std::cerr << "Failed to get a transform between the odom frame and the body frame. "
                     "Returning an empty robot command."
                  << std::endl;
        ::bosdyn::api::RobotCommand command;
        return {::bosdyn::common::Status(
                    SDKErrorCode::GenericSDKError,
                    "Failed to find the odom_tform_body transform in the robot state snapshot."),
                command};
    }
    auto odom_tform_goto = odom_tform_body * goto_rt_body;
    return {::bosdyn::common::Status(SDKErrorCode::Success),
            TrajectoryCommand(odom_tform_goto, ::bosdyn::api::kOdomFrame, params)};
}

Result<::bosdyn::api::RobotCommand> TrajectoryCommandInBodyFrame(
    const double& goal_x_rt_body, const double& goal_y_rt_body, const double& goal_heading_rt_body,
    const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot) {
    ::bosdyn::api::spot::MobilityParams empty_params;
    return TrajectoryCommandInBodyFrame(goal_x_rt_body, goal_y_rt_body, goal_heading_rt_body,
                                        frame_tree_snapshot, empty_params);
}

::bosdyn::api::RobotCommand VelocityCommand(const double& vel_x, const double& vel_y,
                                            const double vel_rot, const std::string& se2_frame_name,
                                            const ::bosdyn::api::spot::MobilityParams& params) {
    ::bosdyn::api::RobotCommand robot_command;
    robot_command.mutable_synchronized_command()
        ->mutable_mobility_command()
        ->mutable_params()
        ->CopyFrom(ToAny(params));
    robot_command.mutable_synchronized_command()
        ->mutable_mobility_command()
        ->mutable_se2_velocity_request()
        ->set_se2_frame_name(se2_frame_name);
    ::bosdyn::api::SE2Velocity* vel = robot_command.mutable_synchronized_command()
                                          ->mutable_mobility_command()
                                          ->mutable_se2_velocity_request()
                                          ->mutable_velocity();
    vel->mutable_linear()->set_x(vel_x);
    vel->mutable_linear()->set_y(vel_y);
    vel->set_angular(vel_rot);
    // Set a default slew rate limit.
    ::bosdyn::api::SE2Velocity* slew_rate_limit = robot_command.mutable_synchronized_command()
                                                      ->mutable_mobility_command()
                                                      ->mutable_se2_velocity_request()
                                                      ->mutable_slew_rate_limit();
    // Set a maximum, default slew rate value with linear values of 4 m/s and an angular value
    // of 2.0 rad/sec.
    slew_rate_limit->mutable_linear()->set_x(4.0);
    slew_rate_limit->mutable_linear()->set_y(4.0);
    slew_rate_limit->set_angular(2.0);
    return robot_command;
}

::bosdyn::api::RobotCommand VelocityCommand(const double& vel_x, const double& vel_y,
                                            const double vel_rot,
                                            const std::string& se2_frame_name) {
    ::bosdyn::api::spot::MobilityParams empty_params;
    return VelocityCommand(vel_x, vel_y, vel_rot, se2_frame_name, empty_params);
}

void AddGotoToCommand(const ::bosdyn::api::SE2Pose& goto_pt, const std::string& se2_frame_name,
                      ::bosdyn::api::RobotCommand* robot_command) {
    auto trajectory_request = robot_command->mutable_synchronized_command()
                                  ->mutable_mobility_command()
                                  ->mutable_se2_trajectory_request();
    trajectory_request->set_se2_frame_name(se2_frame_name);
    trajectory_request->mutable_trajectory()->add_points()->mutable_pose()->CopyFrom(goto_pt);
}

google::protobuf::Any ToAny(const ::bosdyn::api::spot::MobilityParams& mobility_params) {
    google::protobuf::Any any_params;
    any_params.PackFrom(mobility_params);
    return any_params;
}

::bosdyn::api::RobotCommand ArmNamedCommand(
    const ::bosdyn::api::NamedArmPositionsCommand::Positions position) {
    ::bosdyn::api::RobotCommand robot_command;
    robot_command.mutable_synchronized_command()
        ->mutable_arm_command()
        ->mutable_named_arm_position_command()
        ->set_position(position);
    return robot_command;
}
Result<::bosdyn::api::RobotCommand> ArmNamedCommand(
    const ::bosdyn::api::NamedArmPositionsCommand::Positions position,
    const ::bosdyn::api::RobotCommand& build_on_command) {
    auto arm_named_command = ArmNamedCommand(position);

    auto status = BuildSynchroCommand(build_on_command, &arm_named_command);
    return {status, std::move(arm_named_command)};
}

::bosdyn::api::RobotCommand ArmStowCommand() {
    return ArmNamedCommand(::bosdyn::api::NamedArmPositionsCommand::POSITIONS_STOW);
}
Result<::bosdyn::api::RobotCommand> ArmStowCommand(
    const ::bosdyn::api::RobotCommand& build_on_command) {
    return ArmNamedCommand(::bosdyn::api::NamedArmPositionsCommand::POSITIONS_STOW,
                           build_on_command);
}

::bosdyn::api::RobotCommand ArmReadyCommand() {
    return ArmNamedCommand(::bosdyn::api::NamedArmPositionsCommand::POSITIONS_READY);
}
Result<::bosdyn::api::RobotCommand> ArmReadyCommand(
    const ::bosdyn::api::RobotCommand& build_on_command) {
    return ArmNamedCommand(::bosdyn::api::NamedArmPositionsCommand::POSITIONS_READY,
                           build_on_command);
}

::bosdyn::api::RobotCommand ArmCarryCommand() {
    return ArmNamedCommand(::bosdyn::api::NamedArmPositionsCommand::POSITIONS_CARRY);
}
Result<::bosdyn::api::RobotCommand> ArmCarryCommand(
    const ::bosdyn::api::RobotCommand& build_on_command) {
    return ArmNamedCommand(::bosdyn::api::NamedArmPositionsCommand::POSITIONS_CARRY,
                           build_on_command);
}

::bosdyn::api::RobotCommand ClawGripperOpenAngleCommand(double gripper_q) {
    ::bosdyn::api::RobotCommand robot_command;
    robot_command.mutable_synchronized_command()
        ->mutable_gripper_command()
        ->mutable_claw_gripper_command()
        ->mutable_trajectory()
        ->add_points()
        ->set_point(gripper_q);
    return robot_command;
}
Result<::bosdyn::api::RobotCommand> ClawGripperOpenAngleCommand(
    double gripper_q, const ::bosdyn::api::RobotCommand& build_on_command) {
    auto gripper_command = ClawGripperOpenAngleCommand(gripper_q);

    auto status = BuildSynchroCommand(build_on_command, &gripper_command);
    return {status, std::move(gripper_command)};
}

::bosdyn::api::RobotCommand ClawGripperOpenCommand() {
    return ClawGripperOpenAngleCommand(kClawGripperOpenAngle);
}
Result<::bosdyn::api::RobotCommand> ClawGripperOpenCommand(
    const ::bosdyn::api::RobotCommand& build_on_command) {
    return ClawGripperOpenAngleCommand(kClawGripperOpenAngle, build_on_command);
}

::bosdyn::api::RobotCommand ClawGripperCloseCommand() {
    return ClawGripperOpenAngleCommand(kClawGripperClosedAngle);
}
Result<::bosdyn::api::RobotCommand> ClawGripperCloseCommand(
    const ::bosdyn::api::RobotCommand& build_on_command) {
    return ClawGripperOpenAngleCommand(kClawGripperClosedAngle, build_on_command);
}

::bosdyn::api::RobotCommand ArmPoseCommandFromPose(const ::bosdyn::api::SE3Pose& hand_pose,
                                                   const std::string& frame_name,
                                                   const double seconds) {
    ::bosdyn::api::RobotCommand robot_command;
    ::bosdyn::api::SE3TrajectoryPoint hand_pose_traj_point;
    *(hand_pose_traj_point.mutable_time_since_reference()) =
        ::bosdyn::common::SecToDuration(seconds);
    *(hand_pose_traj_point.mutable_pose()) = hand_pose;
    robot_command.mutable_synchronized_command()
        ->mutable_arm_command()
        ->mutable_arm_cartesian_command()
        ->set_root_frame_name(frame_name);
    *(robot_command.mutable_synchronized_command()
          ->mutable_arm_command()
          ->mutable_arm_cartesian_command()
          ->mutable_pose_trajectory_in_task()
          ->add_points()) = hand_pose_traj_point;
    return robot_command;
}

Result<::bosdyn::api::RobotCommand> ArmPoseCommandFromPose(
    const ::bosdyn::api::RobotCommand& build_on_command, const ::bosdyn::api::SE3Pose& hand_pose,
    const std::string& frame_name, const double seconds) {
    auto arm_pose_command = ArmPoseCommandFromPose(hand_pose, frame_name, seconds);
    auto status = BuildSynchroCommand(build_on_command, &arm_pose_command);
    return {status, std::move(arm_pose_command)};
}

::bosdyn::api::RobotCommand ArmPoseCommand(const double x, const double y, const double z,
                                           const double qw, const double qx, const double qy,
                                           const double qz, const std::string& frame_name,
                                           const double seconds) {
    return ArmPoseCommandFromPose(
        ::bosdyn::api::CreateSE3Pose(::bosdyn::api::CreateQuaternion(qw, qx, qy, qz),
                                     ::bosdyn::api::CreateVec3(x, y, z)),
        frame_name, seconds);
};

Result<::bosdyn::api::RobotCommand> ArmPoseCommand(
    const ::bosdyn::api::RobotCommand& build_on_command, const double x, const double y,
    const double z, const double qw, const double qx, const double qy, const double qz,
    const std::string& frame_name, const double seconds) {
    auto arm_pose_command = ArmPoseCommand(x, y, z, qw, qx, qy, qz, frame_name, seconds);
    auto status = BuildSynchroCommand(build_on_command, &arm_pose_command);
    return {status, std::move(arm_pose_command)};
};

::bosdyn::common::Status BuildSynchroCommand(const ::bosdyn::api::RobotCommand& additional_command,
                                             ::bosdyn::api::RobotCommand* combined_command) {
    if (combined_command->has_full_body_command() || additional_command.has_full_body_command()) {
        return {SDKErrorCode::GenericSDKError,
                "This function only takes RobotCommands containing "
                "mobility or synchro commands."};
    }

    bool has_command = false;
    if (additional_command.has_mobility_command()) {
        *(combined_command->mutable_synchronized_command()->mutable_mobility_command()) =
            additional_command.mobility_command();
        has_command = true;
    } else if (additional_command.has_synchronized_command()) {
        if (additional_command.synchronized_command().has_mobility_command()) {
            *(combined_command->mutable_synchronized_command()->mutable_mobility_command()) =
                additional_command.synchronized_command().mobility_command();
            has_command = true;
        }
        if (additional_command.synchronized_command().has_arm_command()) {
            *(combined_command->mutable_synchronized_command()->mutable_arm_command()) =
                additional_command.synchronized_command().arm_command();
            has_command = true;
        }
        if (additional_command.synchronized_command().has_gripper_command()) {
            *(combined_command->mutable_synchronized_command()->mutable_gripper_command()) =
                additional_command.synchronized_command().gripper_command();
            has_command = true;
        }
    }

    if (has_command) {
        return {SDKErrorCode::Success};
    }
    return {SDKErrorCode::GenericSDKError, "No input commands found."};
}


void SetCommandMetadata(const std::string& client_name, const ::bosdyn::api::Lease& lease,
                        const std::string& clock_identifier,
                        ::bosdyn::api::RobotCommandRequest* req) {
    ::bosdyn::client::SetRequestHeader(client_name, req);
    req->mutable_lease()->CopyFrom(lease);
    req->set_clock_identifier(clock_identifier);
}

}  // namespace client

}  // namespace bosdyn
