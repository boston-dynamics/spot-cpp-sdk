/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include "bosdyn/client/power/power_client.h"
#include "bosdyn/client/robot_command/robot_command_client.h"
#include "bosdyn/client/robot_state/robot_state_client.h"

namespace bosdyn {

namespace client {

namespace power_client_helper {

/**
 * Power off robot motors safely. This function blocks until robot motors safely power off. This
 * means the robot will attempt to sit before powering off.
 *
 * @param command_client (bosdyn::RobotCommandClient): client for calling RobotCommandService
 *                                                     safe power off.
 * @param state_client (bosdyn::RobotStateClient): client for monitoring power state.
 * @param timeout (::bosdyn::common::Duration): Max time this function will block for.
 * @param update_frequency (double): The frequency with which the robot should check if the
 *                                   command has succeeded.
 * @return ::bosdyn::common::Status object with error code, or Success code.
 */
::bosdyn::common::Status SafePowerOffMotors(
     RobotCommandClient* robot_command_client, RobotStateClient* robot_state_client,
     ::bosdyn::common::Duration timeout = std::chrono::seconds(30), double update_frequency = 1.0);

/**
 * Power on robot motors. This function blocks until the command returns success, or the timeout is
 * reached.
 *
 * @param power_client (::bosdyn::api::PowerClient): client for calling power service.
 * @param timeout (::bosdyn::common::Duration): Max time this function will block for.
 * @param update_frequency (double): The frequency with which the robot should check if the
 *                                   command has succeeded.
 * @return ::bosdyn::common::Status object with error code, or Success code.
 */
::bosdyn::common::Status PowerOnMotors(PowerClient* power_client,
                                       ::bosdyn::common::Duration timeout = std::chrono::seconds(30),
                                       double update_frequency = 1.0);

/**
 * Power off robot motors. This function blocks until the command returns success, or the timeout is
 * reached.
 *
 * @param power_client (::bosdyn::api::PowerClient): client for calling power service.
 * @param timeout (::bosdyn::common::Duration): Max time this function will block for.
 * @param update_frequency (double): The frequency with which the robot should check if the
 *                                   command has succeeded.
 * @return ::bosdyn::common::Status object with error code, or Success code.
 */
::bosdyn::common::Status PowerOffMotors(PowerClient* power_client,
                                        ::bosdyn::common::Duration timeout = std::chrono::seconds(30),
                                        double update_frequency = 1.0);

/**
 * Power off robot safely. This function blocks until robot safely powers off. This means the
 * robot will attempt to sit before powering off.
 *
 * @param command_client (bosdyn::RobotCommandClient): client for calling RobotCommandService
 *                                                     safe power off.
 * @param state_client (bosdyn::RobotStateClient): client for monitoring power state.
 * @param power_client (::bosdyn::api::PowerClient): client for calling power service.
 * @param timeout (::bosdyn::common::Duration): Max time this function will block for.
 * @param update_frequency (double): The frequency with which the robot should check if the
 *                                   command has succeeded.
 * @return ::bosdyn::common::Status object with error code, or Success code.
 */
::bosdyn::common::Status SafePowerOffRobot(
     RobotCommandClient* robot_command_client, RobotStateClient* robot_state_client,
     PowerClient* power_client, ::bosdyn::common::Duration timeout = std::chrono::seconds(30),
     double update_frequency = 1.0);

/**
 * Power off the robot computers. Powering off the robot computers will stop API comms.
 *
 * @param power_client (::bosdyn::api::PowerClient): client for calling power service.
 * @param timeout (::bosdyn::common::Duration): Max time this function will block for.
 * @param update_frequency (double): The frequency with which the robot should check if the
 *                                   command has succeeded.
 * @return ::bosdyn::common::Status object with error code, or Success code.
 */
::bosdyn::common::Status PowerOffRobot(PowerClient* power_client, ::bosdyn::common::Duration timeout,
                                       double update_frequency);

/**
 * Power cycle robot safely. This function blocks until robot safely power cycles. This means the
 * robot will attempt to sit before restarting.
 *
 * @param command_client (bosdyn::RobotCommandClient): client for calling RobotCommandService
 *                                                     safe power off.
 * @param state_client (bosdyn::RobotStateClient): client for monitoring power state.
 * @param power_client (::bosdyn::api::PowerClient): client for calling power service.
 * @param timeout (::bosdyn::common::Duration): Max time this function will block for.
 * @param update_frequency (double): The frequency with which the robot should check if the
 *                                   command has succeeded.
 * @return ::bosdyn::common::Status object with error code, or Success code.
 */
::bosdyn::common::Status SafePowerCycleRobot(
     RobotCommandClient* robot_command_client, RobotStateClient* robot_state_client,
     PowerClient* power_client, ::bosdyn::common::Duration timeout = std::chrono::seconds(30),
     double update_frequency = 1.0);

/**
 * Power cycle the robot computers. Power cycling the robot computers will stop API comms.
 *
 * @param power_client (bosdyn.api.PowerClient): client for calling power service.
 * @param timeout (::bosdyn::common::Duration): Max time this function will block for.
 * @param update_frequency (double): The frequency with which the robot should check if the command
                                     has succeeded.
 * @return ::bosdyn::common::Status object with error code, or Success code.
 */
::bosdyn::common::Status PowerCycleRobot(
     PowerClient* power_client, ::bosdyn::common::Duration timeout = std::chrono::seconds(30),
     double update_frequency=1.0);

/**
 * Power off the robot payload ports.
 *
 * @param power_client (bosdyn.api.PowerClient): client for calling power service.
 * @param timeout (::bosdyn::common::Duration): Max time this function will block for.
 * @param update_frequency (double): The frequency with which the robot should check if the command
                                     has succeeded.
 * @return ::bosdyn::common::Status object with error code, or Success code.
 */
::bosdyn::common::Status PowerOffPayloadPorts(
     PowerClient* power_client, ::bosdyn::common::Duration timeout = std::chrono::seconds(30),
     double update_frequency=1.0);

/**
 * Power on the robot payload ports.
 *
 * @param power_client (bosdyn.api.PowerClient): client for calling power service.
 * @param timeout (::bosdyn::common::Duration): Max time this function will block for.
 * @param update_frequency (double): The frequency with which the robot should check if the command
                                     has succeeded.
 * @return ::bosdyn::common::Status object with error code, or Success code.
 */
::bosdyn::common::Status PowerOnPayloadPorts(
     PowerClient* power_client, ::bosdyn::common::Duration timeout = std::chrono::seconds(30),
     double update_frequency=1.0);

/**
 * Power off the robot Wi-Fi radio.
 *
 * @param power_client (bosdyn.api.PowerClient): client for calling power service.
 * @param timeout (::bosdyn::common::Duration): Max time this function will block for.
 * @param update_frequency (double): The frequency with which the robot should check if the command
                                     has succeeded.
 * @return ::bosdyn::common::Status object with error code, or Success code.
 */
::bosdyn::common::Status PowerOffWifiRadio(
     PowerClient* power_client, ::bosdyn::common::Duration timeout = std::chrono::seconds(30),
     double update_frequency=1.0);

/**
 * Power on the robot Wi-Fi radio.
 *
 * @param power_client (bosdyn.api.PowerClient): client for calling power service.
 * @param timeout (::bosdyn::common::Duration): Max time this function will block for.
 * @param update_frequency (double): The frequency with which the robot should check if the command
                                     has succeeded.
 * @return ::bosdyn::common::Status object with error code, or Success code.
 */
::bosdyn::common::Status PowerOnWifiRadio(
     PowerClient* power_client, ::bosdyn::common::Duration timeout = std::chrono::seconds(30),
     double update_frequency=1.0);



/**
 * Check if the robot is powered on. This function blocks until the robot state is received.
 *
 * @param robot_state_client (::bosdyn::api::RobotStateClient): client for calling robot_state
 *                                                            service.
 * @return Result with ::bosdyn::common::Status object with error code if errors are found, or a Success Status
 *         object and a bool response whether the robot is powered on or not, if no errors are
 *         found.
 */
Result<bool> IsPoweredOn(RobotStateClient* robot_state_client);

/**
 * Send power command to the robot. This function blocks until the command feedback returns
 * success, or the timeout is reached.
 *
 * @param power_client (::bosdyn::api::PowerClient): client for calling power service.
 * @param timeout (::bosdyn::common::Duration): Max time this function will block for.
 * @param update_frequency (double): The frequency with which the robot should check if the
 *                                   command has succeeded.
 * @return ::bosdyn::common::Status object with error code, or Success code.
 */
::bosdyn::common::Status PowerCommand(PowerClient* power_client,
                                      ::bosdyn::api::PowerCommandRequest& request,
                                      ::bosdyn::common::Duration timeout = std::chrono::seconds(30),
                                      double update_frequency = 1.0);
}  // namespace power_client_helper

}  // namespace client

}  // namespace bosdyn
