/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

// A set of free helper functions to be used with a robot command client.

#include "robot_command_client.h"

namespace bosdyn {

namespace client {

// Possible error codes related to issuing a RobotCommand and waiting until the command completes
// before doing anything else.
enum class BlockingRobotCommandErrorCode {
    Success = 0,               // The issued RobotCommand finished successfully.
    CommandFeedbackError = 1,  // The issued RobotCommand finished with an error.
    CommandTimeoutError = 2,   // The issued RobotCommand didn't complete.
};

std::error_code make_error_code(BlockingRobotCommandErrorCode);

/**
 * Blocks until the arm achieves a finishing state for the specific arm command. This helper will
 * block and check the feedback for ArmCartesianCommand, GazeCommand, ArmJointMoveCommand,
 * NamedArmPositionsCommand, and ArmImpedanceCommand.
 *
 * @param robot_command_client (bosdyn.client.RobotCommandClient): The robot command client, used to
 * request command feedback.
 * @param cmd_id (int): The command ID returned by the robot when the arm movement command was sent.
 * @param timeout_sec (int): The number of seconds after which we'll return no matter what the
 * robot's state is. If unset, 0, or negative, this function will never time out and only return
 * when there is finished command feedback.
 * @param poll_period_msec (int): The number of milliseconds to wait between requesting feedback
 * updates. Default is 100 ms.
 *
 * @return True if successfully got to the end of the trajectory, False if the arm stalled or the
 * move was canceled (the arm failed to reach the goal). See the proto definitions in
 * arm_command.proto for more information about why a trajectory would succeed or fail.
 */
RobotCommandFeedbackResultType BlockUntilArmArrives(
    ::bosdyn::client::RobotCommandClient& robot_command_client, int cmd_id, int timeout_sec = 0,
    int poll_period_msec = 100);

/**
 * Blocks until the gripper achieves a finishing state for a ClawGripperCommand.
 *
 * @param robot_command_client (bosdyn.client.RobotCommandClient): The robot command client, used to
 * request command feedback.
 * @param cmd_id (int): The command ID returned by the robot when the gripper movement command was
 * sent.
 * @param timeout_sec (int): The number of seconds after which we'll return no matter what the
 * robot's state is. If unset, 0, or negative, this function will never time out and only return
 * when there is finished command feedback.
 * @param poll_period_msec (int): The number of milliseconds to wait between requesting feedback
 * updates. Default is 100 ms.
 *
 * @return True if the gripper successfully reached the goal or is applying force on something,
 * False if the gripper failed to reach the goal.
 */
RobotCommandFeedbackResultType BlockUntilGripperArrives(
    ::bosdyn::client::RobotCommandClient& robot_command_client, int cmd_id, int timeout_sec = 0,
    int poll_period_msec = 100);
}  // namespace client

}  // namespace bosdyn

namespace std {
template <>
struct is_error_code_enum<::bosdyn::client::BlockingRobotCommandErrorCode> : true_type {};
}  // namespace std
