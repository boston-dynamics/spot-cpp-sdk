/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "robot_command_builder.h"

#include <bosdyn/api/robot_command.pb.h>
#include <bosdyn/api/spot/robot_command.pb.h>

#include "bosdyn/client/service_client/result.h"
#include "bosdyn/client/service_client/client_header_handling.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"
#include "bosdyn/math/frame_helpers.h"
#include "bosdyn/math/proto_math.h"
#include "bosdyn/client/robot_state/robot_state_client.h"

namespace bosdyn {

namespace client {

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
    bool success = ::bosdyn::api::get_a_tform_b(frame_tree_snapshot, ::bosdyn::api::kOdomFrame,
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
                                          const double vel_rot, const std::string& se2_frame_name) {
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


void SetCommandMetadata(const std::string& client_name, const ::bosdyn::api::Lease& lease,
                        const std::string& clock_identifier, ::bosdyn::api::RobotCommandRequest* req) {
    ::bosdyn::client::SetRequestHeader(client_name, req);
    req->mutable_lease()->CopyFrom(lease);
    req->set_clock_identifier(clock_identifier);
}

}  // namespace client

}  // namespace bosdyn
