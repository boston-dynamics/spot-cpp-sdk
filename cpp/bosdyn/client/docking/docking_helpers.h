/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include "bosdyn/common/time.h"
#include "bosdyn/client/docking/docking_client.h"
#include "bosdyn/client/docking/docking_error_codes.h"
#include "bosdyn/common/status.h"
#include "bosdyn/client/robot/robot.h"
#include "bosdyn/client/time_sync/time_sync_helpers.h"

namespace bosdyn {

namespace client {

struct BlockingDockDetails {
    int attempts_made = 0;
};

// Wait on feedback from a docking command.
DockingCommandFeedbackResultType WaitOnFeedback(
    DockingClient* client, uint32_t cmd_id,
    ::bosdyn::api::docking::DockingCommandFeedbackResponse::Status success,
    ::bosdyn::api::docking::DockingCommandFeedbackResponse::Status progress,
    ::bosdyn::common::Duration interval, std::function<bool(void)> early_end);

// Block until the robot has docked, or failed to dock after a number of attempts.
// This can fail with the following codes:
//   CommonError_Code
//   DockingHelperErrorCode
//   RPCErrorCode
// Params:
//   num_attempts: Number of dock attempts to make. A value of <= 0 will make unlimited attempts.
//   interval: How often to check the early_end callback.
//   end_duration: The time added to each command to get its end_time.
//   early_end: This function should return true if the docking attempt should be aborted.
//   cmd_id_given: This function will be called with the command ID each time a new command is made.
Result<BlockingDockDetails> BlockingDock(DockingClient* client,
                                         TimeSyncEndpoint* time_sync_endpoint, unsigned int dock_id,
                                         int num_attempts, ::bosdyn::common::Duration interval,
                                         ::bosdyn::common::Duration end_duration,
                                         std::function<bool(void)> early_end = nullptr,
                                         std::function<void(uint32_t)> cmd_id_given = nullptr);

// BlockingDock that takes a Robot.
// Behaves like the other version, but can also fail with the additional codes:
//   SDKErrorCode
Result<BlockingDockDetails> BlockingDock(Robot* robot, unsigned int dock_id,
                                         int num_attempts,
                                         ::bosdyn::common::Duration interval,
                                         ::bosdyn::common::Duration end_duration,
                                         std::function<bool(void)> early_end = nullptr,
                                         std::function<void(uint32_t)> cmd_id_given = nullptr);
}  // namespace client

}  // namespace bosdyn
