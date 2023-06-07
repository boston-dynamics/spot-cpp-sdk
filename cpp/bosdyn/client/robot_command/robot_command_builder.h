/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

// A set of free functions to be used by a client to the robot command service.
#include <string>

#include <bosdyn/api/arm_command.pb.h>
#include <bosdyn/api/basic_command.pb.h>
#include <bosdyn/api/geometry.pb.h>
#include <bosdyn/api/lease.pb.h>
#include <bosdyn/api/robot_command.pb.h>
#include <bosdyn/api/robot_state.pb.h>
#include <bosdyn/api/spot/robot_command.pb.h>
#include <bosdyn/api/synchronized_command.pb.h>
#include <bosdyn/api/trajectory.pb.h>

#include <bosdyn/api/robot_command_service.pb.h>
#include "robot_command_client.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"
#include "bosdyn/client/service_client/result.h"
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
 * @param params (bosdyn::api::spot::MobilityParams): [Optional] Mobility parameters which can be
 * used to specify things like disabled perception.
 *
 * @return RobotCommand protobuf message, which can be issued to the robot command service.
 */
::bosdyn::api::RobotCommand SitCommand(const ::bosdyn::api::spot::MobilityParams& params);
::bosdyn::api::RobotCommand SitCommand();

/**
 * Command robot to stand. If the robot is sitting, it will stand up. If the robot is
 * moving, it will come to a stop.
 *
 * @param params (bosdyn::api::spot::MobilityParams): [Optional] Mobility parameters which can be
 * used to specify a trajectory for the body to follow while standing. In the simplest case, this
 * can be a specific position+orientation which the robot body will hold at.
 *
 * @return RobotCommand protobuf message, which can be issued to the robot command service.
 */
::bosdyn::api::RobotCommand StandCommand(const ::bosdyn::api::spot::MobilityParams& params);
::bosdyn::api::RobotCommand StandCommand();

/**
 * Create a BodyControlParams.BodyPose from a single desired `body_pose` relative to `frame_name`.
 *
 * @param frame_name (string): The root frame for the desired pose.
 * @param body_pose (bosdyn::api::SE3Pose): The desired 3D body pose for the robot to stand in.
 *
 * @return spot.BodyControlParams.BodyPose, which specifies the desired body pose for a
 * StandCommand.
 */
::bosdyn::api::spot::BodyControlParams::BodyPose BodyPose(const std::string& frame_name,
                                                          const ::bosdyn::api::SE3Pose& body_pose);

/**
 * Command robot to move to a destination pose along a 2D plane.
 *
 * @param goto_pose (bosdyn::api::SE2Pose): The desired 2D pose that the robot will walk to as the
 * goal point of a synchronized trajectory command.
 * @param se2_frame_name (string): The frame name which the goto_pose is described in; this can be
 * odom (kinematic odometry) frame or the vision frame.
 * @param params (bosdyn::api::spot::MobilityParams): [Optional] Mobility parameters which can be
 * used to specify things like the locomotion or speed limits.
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
 * @param params (bosdyn::api::spot::MobilityParams): [Optional] Mobility parameters which can be
 * used to specify things like the locomotion or speed limits.
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
 * @param frame_tree_snapshot (bosdyn::api::FrameTreeSnapshot): The frame tree snapshot, which
 * contains all the different frames and transforms known to the robot, such that the body based
 * command can be converted to a non-moving world frame.
 * @param params (bosdyn::api::spot::MobilityParams): [Optional] Mobility parameters which can be
 * used to specify things like the locomotion or speed limits.
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
 * @param params (bosdyn::api::spot::MobilityParams): [Optional] Mobility parameters which can be
 * used to specify things like the locomotion or speed limits.
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
 * @param goto_pt (bosdyn::api::SE2Pose): The goal point for the synchronized trajectory command.
 * @param se2_frame_name (string): The frame name which the goto_pose is described in; this can be
 * odom (kinematic odometry) frame or the vision frame.
 * @param robot_command (bosdyn::api::RobotCommand): The robot command message that will be mutated
 * to include the frame name and goal point.
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
 * @param params (bosdyn::api::spot::MobilityParams): The mobility parameters message to be packaged
 * into the Any message.
 *
 * @return The protobuf Any message that contains the MobilityParameters.
 */
google::protobuf::Any ToAny(const ::bosdyn::api::spot::MobilityParams& mobility_params);

// Helper methods for using the SpotArm.

/**
 * Command the arm to a named position, defined in arm_command.proto.
 *
 * @param position (bosdyn::api::NamedArmPositionsCommand::Positions): The named position to send
 * the arm to.
 *
 * @return RobotCommand protobuf message, which can be issued to the robot command service.
 */
::bosdyn::api::RobotCommand ArmNamedCommand(
    const ::bosdyn::api::NamedArmPositionsCommand::Positions position);
/**
 * Command the arm to a named position, defined in arm_command.proto.
 *
 * @param position (bosdyn::api::NamedArmPositionsCommand::Positions): The named position to send
 * the arm to.
 * @param build_on_command (bosdyn::api::RobotCommand): Option to input a RobotCommand (not
 * containing a full_body_command). A mobility_command and gripper_command from this incoming
 * RobotCommand will be added to the returned arm RobotCommand.
 *
 * @return RobotCommand protobuf message wrapped in a result type, which can be issued to the robot
 * command service. If build_on_command is not a valid command to send to BuildSynchroCommand, the
 * resulting RobotCommand will be empty and the status will be an error.
 */
Result<::bosdyn::api::RobotCommand> ArmNamedCommand(
    const ::bosdyn::api::NamedArmPositionsCommand::Positions position,
    const ::bosdyn::api::RobotCommand& build_on_command);

/**
 * Command the arm to stow.
 *
 * @return RobotCommand protobuf message, which can be issued to the robot command service.
 */
::bosdyn::api::RobotCommand ArmStowCommand();
/**
 * Command the arm to stow.
 *
 * @param build_on_command (bosdyn::api::RobotCommand): Option to input a RobotCommand (not
 * containing a full_body_command). A mobility_command and gripper_command from this incoming
 * RobotCommand will be added to the returned stow RobotCommand.
 *
 * @return RobotCommand protobuf message wrapped in a result type, which can be issued to the robot
 * command service. If build_on_command is not a valid command to send to BuildSynchroCommand, the
 * resulting RobotCommand will be empty and the status will be an error.
 */
Result<::bosdyn::api::RobotCommand> ArmStowCommand(
    const ::bosdyn::api::RobotCommand& build_on_command);

/**
 * Command the arm to ready.
 *
 * @return RobotCommand protobuf message, which can be issued to the robot command service.
 */
::bosdyn::api::RobotCommand ArmReadyCommand();
/**
 * Command the arm to ready.
 *
 * @param build_on_command (bosdyn::api::RobotCommand): Option to input a RobotCommand (not
 * containing a full_body_command). A mobility_command and gripper_command from this incoming
 * RobotCommand will be added to the returned ready RobotCommand.
 *
 * @return RobotCommand protobuf message wrapped in a result type, which can be issued to the robot
 * command service. If build_on_command is not a valid command to send to BuildSynchroCommand, the
 * resulting RobotCommand will be empty and the status will be an error.
 */
Result<::bosdyn::api::RobotCommand> ArmReadyCommand(
    const ::bosdyn::api::RobotCommand& build_on_command);

/**
 * Command the arm to carry.
 *
 * @return RobotCommand protobuf message, which can be issued to the robot command service.
 */
::bosdyn::api::RobotCommand ArmCarryCommand();
/**
 * Command the arm to carry.
 *
 * @param build_on_command (bosdyn::api::RobotCommand): Option to input a RobotCommand (not
 * containing a full_body_command). A mobility_command and gripper_command from this incoming
 * RobotCommand will be added to the returned carry RobotCommand.
 *
 * @return RobotCommand protobuf message wrapped in a result type, which can be issued to the robot
 * command service. If build_on_command is not a valid command to send to BuildSynchroCommand, the
 * resulting RobotCommand will be empty and the status will be an error.
 */
Result<::bosdyn::api::RobotCommand> ArmCarryCommand(
    const ::bosdyn::api::RobotCommand& build_on_command);

/**
 * Command the gripper to open to a specified angle (in radians).
 *
 * @param gripper_q (float): The angle to move the gripper to, in radians.
 *
 * @return RobotCommand protobuf message, which can be issued to the robot command service.
 */
::bosdyn::api::RobotCommand ClawGripperOpenAngleCommand(double gripper_q);
/**
 * Command the gripper to open to a specified angle (in radians).
 *
 * @param gripper_q (float): The angle to move the gripper to, in radians.
 * @param build_on_command (bosdyn::api::RobotCommand): Option to input a RobotCommand (not
 * containing a full_body_command). A mobility_command and arm_command from this incoming
 * RobotCommand will be added to the returned gripper RobotCommand.
 *
 * @return RobotCommand protobuf message wrapped in a result type, which can be issued to the robot
 * command service. If build_on_command is not a valid command to send to BuildSynchroCommand, the
 * resulting RobotCommand will be empty and the status will be an error.
 */
Result<::bosdyn::api::RobotCommand> ClawGripperOpenAngleCommand(
    double gripper_q, const ::bosdyn::api::RobotCommand& build_on_command);

/**
 * Command the gripper to open all the way.
 *
 * @return RobotCommand protobuf message, which can be issued to the robot command service.
 */
::bosdyn::api::RobotCommand ClawGripperOpenCommand();
/**
 * Command the gripper to open all the way.
 *
 * @param build_on_command (bosdyn::api::RobotCommand): Option to input a RobotCommand (not
 * containing a full_body_command). A mobility_command and arm_command from this incoming
 * RobotCommand will be added to the returned gripper RobotCommand.
 *
 * @return RobotCommand protobuf message wrapped in a result type, which can be issued to the robot
 * command service. If build_on_command is not a valid command to send to BuildSynchroCommand, the
 * resulting RobotCommand will be empty and the status will be an error.
 */
Result<::bosdyn::api::RobotCommand> ClawGripperOpenCommand(
    const ::bosdyn::api::RobotCommand& build_on_command);

/**
 * Command the gripper to close all the way.
 *
 * @return RobotCommand protobuf message, which can be issued to the robot command service.
 */
::bosdyn::api::RobotCommand ClawGripperCloseCommand();
/**
 * Command the gripper to close all the way.
 *
 * @param build_on_command (bosdyn::api::RobotCommand): Option to input a RobotCommand (not
 * containing a full_body_command). A mobility_command and arm_command from this incoming
 * RobotCommand will be added to the returned gripper RobotCommand.
 *
 * @return RobotCommand protobuf message wrapped in a result type, which can be issued to the robot
 * command service. If build_on_command is not a valid command to send to BuildSynchroCommand, the
 * resulting RobotCommand will be empty and the status will be an error.
 */
Result<::bosdyn::api::RobotCommand> ClawGripperCloseCommand(
    const ::bosdyn::api::RobotCommand& build_on_command);

/**
 * Builds an SE3Trajectory Point to tell robot arm to move to a pose in space relative to the frame
 * specified. Wraps it in SynchronizedCommand.
 *
 * @param x (double): The x-component of the desired hand trajectory position, in meters.
 * @param y (double): The y-component of the desired hand trajectory position, in meters.
 * @param z (double): The z-component of the desired hand trajectory position, in meters.
 * @param qw (double): The w-component of the desired hand trajectory orientation, in radians.
 * @param qx (double): The x-component of the desired hand trajectory orientation, in radians.
 * @param qy (double): The y-component of the desired hand trajectory orientation, in radians.
 * @param qz (double): The z-component of the desired hand trajectory orientation, in radians.
 * @param frame_name (string): The frame the pose is relative to.
 * @param seconds (double): The duration to reach the point.
 *
 * @return RobotCommand protobuf message, which can be issued to the robot command service.
 */
::bosdyn::api::RobotCommand ArmPoseCommand(const double x, const double y, const double z,
                                           const double qw, const double qx, const double qy,
                                           const double qz, const std::string& frame_name,
                                           const double seconds = 5.0);
/**
 * Builds an SE3Trajectory Point to tell robot arm to move to a pose in space relative to the frame
 * specified. Wraps it in SynchronizedCommand.
 *
 * @param build_on_command (bosdyn::api::RobotCommand): Option to input a RobotCommand (not
 * containing a full_body_command). A mobility_command and gripper_command from this incoming
 * RobotCommand will be added to the returned arm RobotCommand.
 * @param x (double): The x-component of the desired hand trajectory position, in meters.
 * @param y (double): The y-component of the desired hand trajectory position, in meters.
 * @param z (double): The z-component of the desired hand trajectory position, in meters.
 * @param qw (double): The w-component of the desired hand trajectory orientation, in radians.
 * @param qx (double): The x-component of the desired hand trajectory orientation, in radians.
 * @param qy (double): The y-component of the desired hand trajectory orientation, in radians.
 * @param qz (double): The z-component of the desired hand trajectory orientation, in radians.
 * @param frame_name (string): The frame the pose is relative to.
 * @param seconds (double): The duration to reach the point.
 *
 * @return RobotCommand protobuf message wrapped in a result type, which can be issued to the robot
 * command service. If build_on_command is not a valid command to send to BuildSynchroCommand, the
 * resulting RobotCommand will be empty and the status will be an error.
 */
Result<::bosdyn::api::RobotCommand> ArmPoseCommand(
    const ::bosdyn::api::RobotCommand& build_on_command, const double x, const double y,
    const double z, const double qw, const double qx, const double qy, const double qz,
    const std::string& frame_name, const double seconds = 5.0);

/**
 * Builds an SE3Trajectory Point to tell robot arm to move to a pose in space relative to the frame
 * specified. Wraps it in SynchronizedCommand.
 *
 * @param hand_pose (bosdyn::api::SE3Pose): The desired hand trajectory point.
 * @param frame_name (string): The frame the pose is relative to.
 * @param seconds (double): The duration to reach the point.
 *
 * @return RobotCommand protobuf message, which can be issued to the robot command service.
 */
::bosdyn::api::RobotCommand ArmPoseCommandFromPose(const ::bosdyn::api::SE3Pose& hand_pose,
                                                   const std::string& frame_name,
                                                   const double seconds = 5.0);
/**
 * Builds an SE3Trajectory Point to tell robot arm to move to a pose in space relative to the frame
 * specified. Wraps it in SynchronizedCommand.
 *
 * @param build_on_command (bosdyn::api::RobotCommand): Option to input a RobotCommand (not
 * containing a full_body_command). A mobility_command and gripper_command from this incoming
 * RobotCommand will be added to the returned arm RobotCommand.
 * @param hand_pose (bosdyn::api::SE3Pose): The desired hand trajectory point.
 * @param frame_name (string): The frame the pose is relative to.
 * @param seconds (double): The duration to reach the point.
 *
 * @return RobotCommand protobuf message wrapped in a result type, which can be issued to the robot
 * command service. If build_on_command is not a valid command to send to BuildSynchroCommand, the
 * resulting RobotCommand will be empty and the status will be an error.
 */
Result<::bosdyn::api::RobotCommand> ArmPoseCommandFromPose(
    const ::bosdyn::api::RobotCommand& build_on_command, const ::bosdyn::api::SE3Pose& hand_pose,
    const std::string& frame_name, const double seconds = 5.0);

/**
 * Combines multiple commands into one command. There's no intelligence here on duplicate
 * commands.
 *
 * @param additional_command (bosdyn::api::RobotCommand): A RobotCommand containing only either
 * mobility commands or synchro commands that will be combined into combined_command.
 * @param combined_command (bosdyn::api::RobotCommand*): A pointer to a RobotCommand containing
 * either mobility commands or synchro commands that will be modified to include the mobility or
 * synchro command in additional_command.
 *
 * @return A Status indicating if combining the commands was successful. If input_command or
 * additional_command contains a full body command, or neither of them contains any mobility or
 * synchro commands, the Status code will be an error, and input_command will not be modified.
 */
::bosdyn::common::Status BuildSynchroCommand(const ::bosdyn::api::RobotCommand& additional_command,
                                             ::bosdyn::api::RobotCommand* combined_command);


/**
 * Set generic information common to all robot command calls.
 *
 * @param client_name (string): The client name creating the robot command.
 * @param lease (bosdyn::api::Lease): The proto representing the current lease for the resources
 * (i.e. "body") necessary for the command.
 * @param clock_identifier (string): The timesync clock identifier.
 * @param req (bosdyn::api::RobotCommandRequest): The robot command request that will be mutated
 * to included the common parameters.
 *
 * @returns: None. Mutates the robot command request to include the necessary common parameters
 * for the command to be successfully issued to the robot.
 */
void SetCommandMetadata(const std::string& client_name, const ::bosdyn::api::Lease& lease,
                        const std::string& clock_identifier,
                        ::bosdyn::api::RobotCommandRequest* req);

}  // namespace client

}  // namespace bosdyn
