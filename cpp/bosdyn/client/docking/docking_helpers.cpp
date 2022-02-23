/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "docking_helpers.h"
#include <chrono>

#include "bosdyn/client/error_codes/docking_helper_error_code.h"

namespace bosdyn {

namespace client {

DockingCommandFeedbackResultType WaitOnFeedback(
    DockingClient* client, uint32_t cmd_id,
    ::bosdyn::api::docking::DockingCommandFeedbackResponse::Status success,
    ::bosdyn::api::docking::DockingCommandFeedbackResponse::Status progress,
    ::bosdyn::common::Duration interval, std::function<bool(void)> early_end) {
    DockingCommandFeedbackResultType docking_fb_res;

    ::bosdyn::common::Duration sleep_duration(0);
    do {
        // Sleep, to limit requests to at most one per interval.
        std::this_thread::sleep_for(sleep_duration);

        std::shared_future<DockingCommandFeedbackResultType> docking_fb_res_future =
            client->DockingCommandFeedbackAsync(cmd_id);
        ::bosdyn::common::TimePoint rpc_start(::bosdyn::common::NsecSinceEpoch());
        while (!early_end || !early_end()) {
            if (docking_fb_res_future.wait_for(interval) == std::future_status::ready) break;
        }
        // Set sleep duration for the next loop.
        sleep_duration = interval - (
            ::bosdyn::common::TimePoint(::bosdyn::common::NsecSinceEpoch()) - rpc_start);
        docking_fb_res = docking_fb_res_future.get();

        // Keep checking feedback if the RPC failed or the response indicated "in progress"
        // (and we haven't been told to end early)
    } while ((!docking_fb_res || docking_fb_res.response.status() == progress) &&
             (!early_end || !early_end()));

    if (docking_fb_res.response.status() == success) {
        // If we got the "success" status, we just return the whole Result.
        return docking_fb_res;
    } else if (docking_fb_res) {
        // If we got a non-success status, but the Result indicates success, override the Status
        // code to indicate the command we were looking at failed.
        return {::bosdyn::common::Status(DockingHelperErrorCode::CommandFailed), docking_fb_res.response};
    }
    // Otherwise (non-success status, non-success Result) we can return the whole Result.
    return docking_fb_res;
}

Result<BlockingDockDetails> BlockingDock(DockingClient* client,
                                         TimeSyncEndpoint* time_sync_endpoint, unsigned int dock_id,
                                         int num_attempts, ::bosdyn::common::Duration interval,
                                         ::bosdyn::common::Duration end_duration,
                                         std::function<bool(void)> early_end,
                                         std::function<void(uint32_t)> cmd_id_given) {
    // How many attempts have we made?
    BlockingDockDetails details;

    while (num_attempts <= 0 || details.attempts_made < num_attempts) {
        // Build the command.
        const auto end_time = ::bosdyn::common::NsecSinceEpoch() + end_duration;
        auto docking_command = client->DockingCommandBuilder(
            dock_id, ::bosdyn::common::TimePoint(end_time), time_sync_endpoint);

        // Can't even build the command; go home in shame.
        if (!docking_command) {
            return {docking_command.status, details};
        }

        // Before we send another command, see if we need to stop.
        if (early_end && early_end()) {
            return {::bosdyn::common::Status(DockingHelperErrorCode::Cancelled), details};
        }

        // Send the command.
        ++details.attempts_made;
        // For the first attempt, and every OTHER attempt after that, use the prep pose.
        const ::bosdyn::api::docking::PrepPoseBehavior pose =
            (details.attempts_made) % 2 == 1 ? ::bosdyn::api::docking::PrepPoseBehavior::PREP_POSE_USE_POSE
                                             : ::bosdyn::api::docking::PrepPoseBehavior::PREP_POSE_SKIP_POSE;
        docking_command.response.set_prep_pose_behavior(pose);
        auto docking_cmd_res_future = client->DockingCommandAsync(docking_command.response);

        // Wait for the RPC to complete, ending early if we've been asked to do so.
        while (true) {
            if (early_end && early_end()) {
                return {::bosdyn::common::Status(DockingHelperErrorCode::Cancelled), details};
            }
            if (docking_cmd_res_future.wait_for(interval) == std::future_status::ready) break;
        }
        DockingCommandResultType docking_cmd_res = docking_cmd_res_future.get();

        // If RPC succeeded, wait on feedback.
        if (docking_cmd_res) {
            // Wait on feedback.
            const uint32_t cmd_id = docking_cmd_res.response.docking_command_id();

            // Now that we have a command ID, pass it to the user's callback.
            if (cmd_id_given) cmd_id_given(cmd_id);

            // Wait for feedback on that command.
            const auto docking_fb_res = WaitOnFeedback(
                client, cmd_id, ::bosdyn::api::docking::DockingCommandFeedbackResponse::STATUS_DOCKED,
                ::bosdyn::api::docking::DockingCommandFeedbackResponse::STATUS_IN_PROGRESS, interval,
                early_end);

            // If feedback indicated a successful dock, return success!
            if (docking_fb_res) {
                return {::bosdyn::common::Status(SDKErrorCode::Success), details};
            }
        }
        // Otherwise, we go around again.
    }

    // Failed to dock after num_attempts. Go to prep pose, to guarantee we are not awkwardly
    // standing over the dock.
    const auto end_time = ::bosdyn::common::NsecSinceEpoch() + end_duration;
    Result<::bosdyn::api::docking::DockingCommandRequest> prep_command = client->DockingCommandBuilder(
        dock_id, ::bosdyn::common::TimePoint(end_time), time_sync_endpoint);
    if (!prep_command) {
        return {prep_command.status, details};
    }
    prep_command.response.set_prep_pose_behavior(
        ::bosdyn::api::docking::PrepPoseBehavior::PREP_POSE_ONLY_POSE);
    auto prep_cmd_res_future = client->DockingCommandAsync(prep_command.response);
    while (true) {
        if (early_end && early_end()) {
            return {::bosdyn::common::Status(DockingHelperErrorCode::Cancelled), details};
        }
        if (prep_cmd_res_future.wait_for(interval) == std::future_status::ready) break;
    }
    DockingCommandResultType prep_cmd_res = prep_cmd_res_future.get();
    if (!prep_cmd_res) {
        return {prep_cmd_res.status, details};
    }
    // Wait on feedback.
    const uint32_t cmd_id = prep_cmd_res.response.docking_command_id();
    // Pass the command id to the user's callback.
    if (cmd_id_given) cmd_id_given(cmd_id);
    const auto docking_fb_res = WaitOnFeedback(
        client, cmd_id, ::bosdyn::api::docking::DockingCommandFeedbackResponse::STATUS_AT_PREP_POSE,
        ::bosdyn::api::docking::DockingCommandFeedbackResponse::STATUS_IN_PROGRESS, interval, early_end);
    return {::bosdyn::common::Status(DockingHelperErrorCode::RetriesExceeded), details};
}

Result<BlockingDockDetails> BlockingDock(Robot* robot, unsigned int dock_id,
                                         int num_attempts,
                                         ::bosdyn::common::Duration interval,
                                         ::bosdyn::common::Duration end_duration,
                                         std::function<bool(void)> early_end,
                                         std::function<void(uint32_t)> cmd_id_given) {
    Result<::bosdyn::client::DockingClient*> client_result =
        robot->EnsureServiceClient<::bosdyn::client::DockingClient>(
            ::bosdyn::client::DockingClient::GetDefaultServiceName());
    if (!client_result) {
        return {client_result.status, BlockingDockDetails()};
    }
    Result<::bosdyn::client::TimeSyncEndpoint*> timepoint_result =
        robot->StartTimeSyncAndGetEndpoint();
    if (!timepoint_result) {
        return {timepoint_result.status, BlockingDockDetails()};
    }
    return BlockingDock(client_result.response, timepoint_result.response, dock_id, num_attempts,
                        interval, end_duration, early_end, cmd_id_given);
}

}  // namespace client

}  // namespace bosdyn
