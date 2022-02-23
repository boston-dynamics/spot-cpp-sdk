/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

// A set of free functions to be used by a client to the robot command service.
#include <string>

#include <bosdyn/api/basic_command.pb.h>
#include <bosdyn/api/geometry.pb.h>
#include <bosdyn/api/lease.pb.h>
#include <bosdyn/api/robot_command.pb.h>
#include <bosdyn/api/spot/robot_command.pb.h>
#include <bosdyn/api/synchronized_command.pb.h>
#include <bosdyn/api/trajectory.pb.h>

#include <bosdyn/api/robot_command_service.pb.h>
#include "bosdyn/client/service_client/result.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"
#include "bosdyn/math/api_common_frames.h"

namespace bosdyn {

namespace client {

/**
 * Command to stop with minimal motion. If the robot is walking, it will transition to stand.
 * If the robot is standing or sitting, it will do nothing.
 *
 * @return RobotCommand protobuf message, which can be issued to the robot command service.
 */
::bosdyn::api::RobotCommand StopCommand();

/**
 * Command to freeze all joints at their current positions (no balancing control).
 *
 * @return RobotCommand protobuf message, which can be issued to the robot command service.
 */
::bosdyn::api::RobotCommand FreezeCommand();

/**
 * Command to get the robot in a ready, sitting position. If the robot is on its back, it
 * will attempt to flip over.
 *
 * @return RobotCommand protobuf message, which can be issued to the robot command service.
 */
::bosdyn::api::RobotCommand SelfrightCommand();

/**
 * Command to get robot into a position where it is safe to power down, then power down. If
 * the robot has fallen, it will power down directly. If the robot is not in a safe position,
 * it will get to a safe position before powering down. The robot will not power down until it
 * is in a safe state.
 *
 * @return RobotCommand protobuf message, which can be issued to the robot command service.
 */
::bosdyn::api::RobotCommand SafePowerOffCommand();

/**
 * Command the robot to sit.
 *
 * @param params (bosdyn.api.spot.MobilityParams): [Optional] Mobility parameters which can be used
 * to specify things like disabled perception.
 *
 * @return RobotCommand protobuf message, which can be issued to the robot command service.
 */
::bosdyn::api::RobotCommand SitCommand(const ::bosdyn::api::spot::MobilityParams& params);
::bosdyn::api::RobotCommand SitCommand();

/**
 * Command robot to stand. If the robot is sitting, it will stand up. If the robot is
 * moving, it will come to a stop.
 *
 * @param params (bosdyn.api.spot.MobilityParams): [Optional] Mobility parameters which can be used
 * to specify a trajectory for the body to follow while standing. In the simplest case, this can be
 * a specific position+orientation which the robot body will hold at.
 *
 * @return RobotCommand protobuf message, which can be issued to the robot command service.
 */
::bosdyn::api::RobotCommand StandCommand(const ::bosdyn::api::spot::MobilityParams& params);
::bosdyn::api::RobotCommand StandCommand();

/**
 * Command robot to move to a destination pose along a 2D plane.
 *
 * @param goto_pose (bosdyn.api.SE2Pose): The desired 2D pose that the robot will walk to as the
 * goal point of a synchronized trajectory command.
 * @param se2_frame_name (string): The frame name which the goto_pose is described in; this can be
 * odom (kinematic odometry) frame or the vision frame.
 * @param params (bosdyn.api.spot.MobilityParams): [Optional] Mobility parameters which can be used
 * to specify things like the locomotion or speed limits.
 *
 * @return RobotCommand protobuf message, which can be issued to the robot command service.
 */
::bosdyn::api::RobotCommand TrajectoryCommand(const ::bosdyn::api::SE2Pose& goto_pose,
                                            const std::string& se2_frame_name,
                                            const ::bosdyn::api::spot::MobilityParams& params);
::bosdyn::api::RobotCommand TrajectoryCommand(const ::bosdyn::api::SE2Pose& goto_pose,
                                            const std::string& se2_frame_name);
/**
 * Command robot to move to a destination pose along a 2D plane.
 *
 * @param goal_x (double): X-coordinate of the goal position.
 * @param goal_y (double): Y-coordinate of the goal position.
 * @param goal_heading (double): Desired heading of the robot at the goal point.
 * @param se2_frame_name (string): The frame name which the goto_pose is described in; this can be
 * odom (kinematic odometry) frame or the vision frame.
 * @param params (bosdyn.api.spot.MobilityParams): [Optional] Mobility parameters which can be used
 * to specify things like the locomotion or speed limits.
 *
 * @return RobotCommand protobuf message, which can be issued to the robot command service.
 */
::bosdyn::api::RobotCommand TrajectoryCommand(const double& goal_x, const double& goal_y,
                                            const double& goal_heading,
                                            const std::string& se2_frame_name,
                                            const ::bosdyn::api::spot::MobilityParams& params);
::bosdyn::api::RobotCommand TrajectoryCommand(const double& goal_x, const double& goal_y,
                                            const double& goal_heading,
                                            const std::string& se2_frame_name);

/**
 * Command robot to move to pose described relative to the robots body along a 2D plane. For
 * example, a command to move forward 2 meters at the same heading will have goal_x_rt_body=2.0,
 * goal_y_rt_body=0.0, goal_heading_rt_body=0.0.
 *
 * @param goal_x_rt_body (double): X-coordinate of the goal position (described relative to the
 * robot body).
 * @param goal_y_rt_body (double): Y-coordinate of the goal position (described relative to the
 * robot body).
 * @param goal_heading_rt_body (double): Desired heading of the robot at the goal point (described
 * relative to the robot body).
 * @param frame_tree_snapshot (bosdyn.api.FrameTreeSnapshot): The frame tree snapshot, which
 * contains all the different frames and transforms known to the robot, such that the body based
 * command can be converted to a non-moving world frame.
 * @param params (bosdyn.api.spot.MobilityParams): [Optional] Mobility parameters which can be used
 * to specify things like the locomotion or speed limits.
 *
 * @return RobotCommand protobuf message wrapped in a result type, which can be issued to the robot
 * command service. The go-to point will be converted to a non-moving world frame (odom frame) to be
 * issued to the robot, and this conversion can potentially fail, so the status of the result should
 * be checked.
 */
Result<::bosdyn::api::RobotCommand> TrajectoryCommandInBodyFrame(
    const double& goal_x_rt_body, const double& goal_y_rt_body, const double& goal_heading_rt_body,
    const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot,
    const ::bosdyn::api::spot::MobilityParams& params);
Result<::bosdyn::api::RobotCommand> TrajectoryCommandInBodyFrame(
    const double& goal_x_rt_body, const double& goal_y_rt_body, const double& goal_heading_rt_body,
    const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot);

/**
 * Command robot to move along 2D plane.
 *
 * @param vel_x (double): Linear velocity in the x-axis of the robot body.
 * @param vel_y (double): Linear velocity in the y-axis of the robot body.
 * @param vel_rot (double): Angular velocity of the robot body.
 * @param se2_frame_name (string): The velocity should be specified in the robot body frame.
 * Currently, other frames are not supported.
 * @param params (bosdyn.api.spot.MobilityParams): [Optional] Mobility parameters which can be used
 * to specify things like the locomotion or speed limits.
 *
 * @return RobotCommand protobuf message, which can be issued to the robot command service.
 */
::bosdyn::api::RobotCommand VelocityCommand(const double& vel_x, const double& vel_y,
                                          double vel_rot, const std::string& se2_frame_name,
                                          const ::bosdyn::api::spot::MobilityParams& params);
::bosdyn::api::RobotCommand VelocityCommand(const double& vel_x, const double& vel_y,
                                          double vel_rot, const std::string& se2_frame_name);

/**
 * Sets the goal point and the frame name in a synchronized robot command's trajectory command.
 *
 * @param goto_pt (bosdyn.api.SE2Pose): The goal point for the synchronized trajectory command.
 * @param se2_frame_name (string): The frame name which the goto_pose is described in; this can be
 * odom (kinematic odometry) frame or the vision frame.
 * @param robot_command (bosdyn.api.RobotCommand): The robot command message that will be mutated to
 * include the frame name and goal point.
 *
 * @returns: None. Mutates the robot command message to append the goal point and frame name for the
 * trajectory command.
 *
 */
void AddGotoToCommand(const ::bosdyn::api::SE2Pose& goto_pt, const std::string& se2_frame_name,
                      ::bosdyn::api::RobotCommand* robot_command);

/**
 * Converts the mobility parameters into a google::protobuf::Any message so that they can
 *  easily be added to a robot command message.
 *
 * @param params (bosdyn.api.spot.MobilityParams): The mobility parameters message to be packaged
 * into the Any message.
 *
 * @return The protobuf Any message that contains the MobilityParameters.
 */
google::protobuf::Any ToAny(const ::bosdyn::api::spot::MobilityParams& mobility_params);


/**
 * Set generic information common to all robot command calls.
 *
 * @param client_name (string): The client name creating the robot command.
 * @param lease (bosdyn.api.Lease): The proto representing the current lease for the resources (i.e.
 * "body") necessary for the command.
 * @param clock_identifier (string): The timesync clock identifier.
 * @param req (bosdyn.api.RobotCommandRequest): The robot command request that will be mutated to
 * included the common parameters.
 *
 * @returns: None. Mutates the robot command request to include the necessary common parameters for
 * the command to be successfully issued to the robot.
 */
void SetCommandMetadata(const std::string& client_name, const ::bosdyn::api::Lease& lease,
                        const std::string& clock_identifier, ::bosdyn::api::RobotCommandRequest* req);

}  // namespace client

}  // namespace bosdyn
