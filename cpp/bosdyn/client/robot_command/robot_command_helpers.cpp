/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include <chrono>

#include <bosdyn/api/arm_command.pb.h>
#include <bosdyn/api/gripper_command.pb.h>

#include "robot_command_helpers.h"
#include "bosdyn/client/error_codes/error_type_condition.h"
#include "bosdyn/client/error_codes/rpc_error_code.h"
#include "bosdyn/common/status.h"
#include "bosdyn/common/success_condition.h"
#include "bosdyn/common/time.h"

namespace bosdyn {
namespace client {
namespace {
struct BlockingRobotCommandErrorCodeCategory : std::error_category {
    const char* name() const noexcept override;
    std::string message(int value) const override;
    bool equivalent(int valcode, const std::error_condition& cond) const noexcept override;
};

bool BlockingRobotCommandErrorCodeCategory::equivalent(
    int valcode, const std::error_condition& cond) const noexcept {
    if (cond == SuccessCondition::Success)
        return (valcode ==
                static_cast<int>(::bosdyn::client::BlockingRobotCommandErrorCode::Success));
    if (cond == ErrorTypeCondition::SDKError) return true;
    return false;
}

const char* BlockingRobotCommandErrorCodeCategory::name() const noexcept {
    return "BlockingRobotCommandErrorCode";
}

std::string BlockingRobotCommandErrorCodeCategory::message(int value) const {
    switch (static_cast<::bosdyn::client::BlockingRobotCommandErrorCode>(value)) {
        case ::bosdyn::client::BlockingRobotCommandErrorCode::Success:
            return "Success";
        case ::bosdyn::client::BlockingRobotCommandErrorCode::CommandFeedbackError:
            return "CommandFeedbackError";
        case ::bosdyn::client::BlockingRobotCommandErrorCode::CommandTimeoutError:
            return "CommandTimeoutError";
    }
    return "(BlockingRobotCommandErrorCode: unrecognized error)";
}

const BlockingRobotCommandErrorCodeCategory BlockingRobotCommandErrorCodeCategory_category{};
}  // namespace

std::error_code make_error_code(BlockingRobotCommandErrorCode value) {
    return {static_cast<int>(value), BlockingRobotCommandErrorCodeCategory_category};
}

const std::error_category& BlockingRobotCommandErrorCategory() {
    return BlockingRobotCommandErrorCodeCategory_category;
}

RobotCommandFeedbackResultType BlockUntilArmArrives(
    ::bosdyn::client::RobotCommandClient& robot_command_client, int cmd_id, int timeout_sec,
    int poll_period_msec) {
    ::bosdyn::api::RobotCommandFeedbackRequest feedback_req;
    feedback_req.set_robot_command_id(cmd_id);

    ::bosdyn::client::RobotCommandFeedbackResultType feedback_res;

    auto start_time = ::bosdyn::common::NowNsec();
    const auto end_time = start_time + ::bosdyn::common::SecToNsec(timeout_sec);

    while (timeout_sec <= 0 || ::bosdyn::common::NowNsec() < end_time) {
        feedback_res = robot_command_client.RobotCommandFeedback(feedback_req);
        // If the RPC timed out but this function hasn't timed out, keep trying.
        if (!feedback_res.status &&
            feedback_res.status.code() != ::bosdyn::client::RPCErrorCode::TimedOutError) {
            return feedback_res;
        }

        auto arm_feedback =
            feedback_res.response.feedback().synchronized_feedback().arm_command_feedback();

        if (arm_feedback.has_arm_cartesian_feedback()) {
            if (arm_feedback.arm_cartesian_feedback().status() ==
                ::bosdyn::api::ArmCartesianCommand::Feedback::STATUS_TRAJECTORY_COMPLETE) {
                return {{BlockingRobotCommandErrorCode::Success}, feedback_res.move()};
            } else if (
                arm_feedback.arm_cartesian_feedback().status() ==
                    ::bosdyn::api::ArmCartesianCommand::Feedback::STATUS_TRAJECTORY_STALLED ||
                arm_feedback.arm_cartesian_feedback().status() ==
                    ::bosdyn::api::ArmCartesianCommand::Feedback::STATUS_TRAJECTORY_CANCELLED) {
                return {{BlockingRobotCommandErrorCode::CommandFeedbackError}, feedback_res.move()};
            }
        } else if (arm_feedback.has_arm_gaze_feedback()) {
            if (arm_feedback.arm_gaze_feedback().status() ==
                ::bosdyn::api::GazeCommand::Feedback::STATUS_TRAJECTORY_COMPLETE) {
                return {{BlockingRobotCommandErrorCode::Success}, feedback_res.move()};
            } else if (arm_feedback.arm_gaze_feedback().status() ==
                       ::bosdyn::api::GazeCommand::Feedback::STATUS_TOOL_TRAJECTORY_STALLED) {
                return {{BlockingRobotCommandErrorCode::CommandFeedbackError}, feedback_res.move()};
            }
        } else if (arm_feedback.has_arm_impedance_feedback()) {
            if (arm_feedback.arm_impedance_feedback().status() ==
                ::bosdyn::api::ArmImpedanceCommand::Feedback::STATUS_TRAJECTORY_COMPLETE) {
                return {{BlockingRobotCommandErrorCode::Success}, feedback_res.move()};
            } else if (arm_feedback.arm_impedance_feedback().status() ==
                       ::bosdyn::api::ArmImpedanceCommand::Feedback::STATUS_TRAJECTORY_STALLED) {
                return {{BlockingRobotCommandErrorCode::CommandFeedbackError}, feedback_res.move()};
            }
        } else if (arm_feedback.has_arm_joint_move_feedback()) {
            if (arm_feedback.arm_joint_move_feedback().status() ==
                ::bosdyn::api::ArmJointMoveCommand::Feedback::STATUS_COMPLETE) {
                return {{BlockingRobotCommandErrorCode::Success}, feedback_res.move()};
            } else if (arm_feedback.arm_joint_move_feedback().status() ==
                       ::bosdyn::api::ArmJointMoveCommand::Feedback::STATUS_STALLED) {
                return {{BlockingRobotCommandErrorCode::CommandFeedbackError}, feedback_res.move()};
            }
        } else if (arm_feedback.has_named_arm_position_feedback()) {
            if (arm_feedback.named_arm_position_feedback().status() ==
                ::bosdyn::api::NamedArmPositionsCommand::Feedback::STATUS_COMPLETE) {
                return {{BlockingRobotCommandErrorCode::Success}, feedback_res.move()};
            } else if (arm_feedback.named_arm_position_feedback().status() ==
                       ::bosdyn::api::NamedArmPositionsCommand::Feedback::
                           STATUS_STALLED_HOLDING_ITEM) {
                return {{BlockingRobotCommandErrorCode::CommandFeedbackError}, feedback_res.move()};
            }
        } else {
            return {{BlockingRobotCommandErrorCode::CommandFeedbackError,
                     "Expected one of the following commands: ArmCartesianCommand, GazeCommand, "
                     "ArmJointMoveCommand, NamedArmPositionsCommand, or ArmImpedanceCommand."},
                    feedback_res.move()};
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(poll_period_msec));
    }
    return {{BlockingRobotCommandErrorCode::CommandTimeoutError, "ArmCommand failed or timed out."},
            feedback_res.response};
}

RobotCommandFeedbackResultType BlockUntilGripperArrives(
    ::bosdyn::client::RobotCommandClient& robot_command_client, int cmd_id, int timeout_sec,
    int poll_period_msec) {
    ::bosdyn::api::RobotCommandFeedbackRequest feedback_req;
    feedback_req.set_robot_command_id(cmd_id);

    ::bosdyn::client::RobotCommandFeedbackResultType feedback_res;

    auto start_time = ::bosdyn::common::NowNsec();
    const auto end_time = start_time + ::bosdyn::common::SecToNsec(timeout_sec);

    while (timeout_sec <= 0 || ::bosdyn::common::NowNsec() < end_time) {
        feedback_res = robot_command_client.RobotCommandFeedback(feedback_req);
        // If the RPC timed out but this function hasn't timed out, keep trying.
        if (!feedback_res.status &&
            feedback_res.status.code() != ::bosdyn::client::RPCErrorCode::TimedOutError) {
            return feedback_res;
        }

        auto gripper_feedback = feedback_res.response.feedback()
                                    .synchronized_feedback()
                                    .gripper_command_feedback()
                                    .claw_gripper_feedback();

        if (gripper_feedback.status() ==
                ::bosdyn::api::ClawGripperCommand::Feedback::STATUS_AT_GOAL ||
            gripper_feedback.status() ==
                ::bosdyn::api::ClawGripperCommand::Feedback::STATUS_APPLYING_FORCE) {
            return {{BlockingRobotCommandErrorCode::Success}, feedback_res.move()};
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(poll_period_msec));
    }
    return {{BlockingRobotCommandErrorCode::CommandTimeoutError,
             "The GripperCommand failed or timed out."},
            feedback_res.move()};
}
}  // namespace client

}  // namespace bosdyn
