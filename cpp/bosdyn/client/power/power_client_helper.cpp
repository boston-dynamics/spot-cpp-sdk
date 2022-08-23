/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/power/power_client_helper.h"
#include "bosdyn/common/assert_precondition.h"
#include "bosdyn/common/time.h"

namespace bosdyn {

namespace client {

namespace power_client_helper {

::bosdyn::common::Status SafePowerOffMotors(RobotCommandClient* robot_command_client,
                                            RobotStateClient* robot_state_client,
                                            ::bosdyn::common::Duration timeout,
                                            double update_frequency) {
    // Make sure the provided arguments are valid.
    // Robot command client and robot state client are required.
    BOSDYN_ASSERT_PRECONDITION(robot_command_client != nullptr,
                               "Robot command client must not be null for SafePowerOff.");
    BOSDYN_ASSERT_PRECONDITION(robot_state_client != nullptr,
                               "Robot state client must not be null for SafePowerOff.");
    // The update frequency cannot be 0 because we use it to compute time.
    // NOTE: We are not erroring on negative update_frequency, it triggers no sleep between tries.
    BOSDYN_ASSERT_PRECONDITION(update_frequency != 0.0,
                               "Update frequency for SafePowerOff feedback cannot be 0.");

    double start_time = ::bosdyn::common::NsecToSec(::bosdyn::common::NowNsec());
    double end_time = start_time + ::bosdyn::common::NsecToSec(timeout.count());
    double update_time = 1.0 / update_frequency;

    ::bosdyn::api::RobotCommandRequest command_request;
    // The set_allocated method is responsible for deleting this memory.
    ::bosdyn::api::SafePowerOffCommand_Request* safe_power_off_request =
        new ::bosdyn::api::SafePowerOffCommand_Request();
    command_request.mutable_command()
        ->mutable_full_body_command()
        ->set_allocated_safe_power_off_request(safe_power_off_request);

    RobotCommandResultType result = robot_command_client->RobotCommand(command_request);
    if (!result) {
        return result.status;
    }

    double start_call_time, call_time;
    std::chrono::duration<double> time_until_timeout, sleep_time;
    std::shared_future<RobotStateResultType> fut_robot_state;
    RobotStateResultType robot_state;
    std::future_status status;
    while (::bosdyn::common::NsecToSec(::bosdyn::common::NowNsec()) < end_time) {
        time_until_timeout = std::chrono::duration<double>(
            end_time - ::bosdyn::common::NsecToSec(::bosdyn::common::NowNsec()));
        start_call_time = ::bosdyn::common::NsecToSec(::bosdyn::common::NowNsec());
        fut_robot_state = robot_state_client->GetRobotStateAsync();
        status = fut_robot_state.wait_for(time_until_timeout);
        if (status == std::future_status::ready) {
            robot_state = fut_robot_state.get();
            if (!robot_state) {
                return robot_state.status;
            }
            if (robot_state.response.robot_state().power_state().motor_power_state() ==
                ::bosdyn::api::PowerState::STATE_OFF) {
                return ::bosdyn::common::Status(SDKErrorCode::Success);
            }
        } else if (status == std::future_status::timeout) {
            return ::bosdyn::common::Status(SDKErrorCode::GenericSDKError, "CommandTimedOutError");
        }

        call_time = ::bosdyn::common::NsecToSec(::bosdyn::common::NowNsec()) - start_call_time;
        sleep_time = std::chrono::duration<double>(std::max(0.0, update_time - call_time));
        std::this_thread::sleep_for(sleep_time);
    }

    return ::bosdyn::common::Status(SDKErrorCode::GenericSDKError, "CommandTimedOutError");
}

::bosdyn::common::Status PowerOnMotors(PowerClient* power_client,
                                       ::bosdyn::common::Duration timeout,
                                       double update_frequency) {
    ::bosdyn::api::PowerCommandRequest command_request;
    command_request.set_request(::bosdyn::api::PowerCommandRequest::REQUEST_ON_MOTORS);
    return PowerCommand(power_client, command_request, timeout, update_frequency);
}

::bosdyn::common::Status PowerOffMotors(PowerClient* power_client,
                                        ::bosdyn::common::Duration timeout,
                                        double update_frequency) {
    ::bosdyn::api::PowerCommandRequest command_request;
    command_request.set_request(::bosdyn::api::PowerCommandRequest::REQUEST_OFF_MOTORS);
    return PowerCommand(power_client, command_request, timeout, update_frequency);
}

::bosdyn::common::Status SafePowerOffRobot(RobotCommandClient* robot_command_client,
                                           RobotStateClient* robot_state_client,
                                           PowerClient* power_client,
                                           ::bosdyn::common::Duration timeout,
                                           double update_frequency) {
    ::bosdyn::common::Duration end_time = ::bosdyn::common::NsecSinceEpoch() + timeout;
    auto safe_power_off_status =
        SafePowerOffMotors(robot_command_client, robot_state_client,
                           end_time - ::bosdyn::common::NsecSinceEpoch(), update_frequency);
    if (!safe_power_off_status) {
        return safe_power_off_status;
    }
    return PowerOffRobot(power_client, end_time - ::bosdyn::common::NsecSinceEpoch(),
                         update_frequency);
}

::bosdyn::common::Status PowerOffRobot(PowerClient* power_client,
                                       ::bosdyn::common::Duration timeout,
                                       double update_frequency) {
    ::bosdyn::api::PowerCommandRequest command_request;
    command_request.set_request(::bosdyn::api::PowerCommandRequest::REQUEST_OFF_ROBOT);
    return PowerCommand(power_client, command_request, timeout, update_frequency);
}

::bosdyn::common::Status SafePowerCycleRobot(RobotCommandClient* robot_command_client,
                                             RobotStateClient* robot_state_client,
                                             PowerClient* power_client,
                                             ::bosdyn::common::Duration timeout,
                                             double update_frequency) {
    ::bosdyn::common::Duration end_time = ::bosdyn::common::NsecSinceEpoch() + timeout;
    auto safe_power_off_status =
        SafePowerOffMotors(robot_command_client, robot_state_client,
                           end_time - ::bosdyn::common::NsecSinceEpoch(), update_frequency);
    if (!safe_power_off_status) {
        return safe_power_off_status;
    }
    return PowerCycleRobot(power_client, end_time - ::bosdyn::common::NsecSinceEpoch(),
                           update_frequency);
}

::bosdyn::common::Status PowerCycleRobot(PowerClient* power_client,
                                         ::bosdyn::common::Duration timeout,
                                         double update_frequency) {
    ::bosdyn::api::PowerCommandRequest command_request;
    command_request.set_request(::bosdyn::api::PowerCommandRequest::REQUEST_CYCLE_ROBOT);
    return PowerCommand(power_client, command_request, timeout, update_frequency);
}

::bosdyn::common::Status PowerOnPayloadPorts(PowerClient* power_client,
                                             ::bosdyn::common::Duration timeout,
                                             double update_frequency) {
    ::bosdyn::api::PowerCommandRequest command_request;
    command_request.set_request(::bosdyn::api::PowerCommandRequest::REQUEST_ON_PAYLOAD_PORTS);
    return PowerCommand(power_client, command_request, timeout, update_frequency);
}

::bosdyn::common::Status PowerOffPayloadPorts(PowerClient* power_client,
                                              ::bosdyn::common::Duration timeout,
                                              double update_frequency) {
    ::bosdyn::api::PowerCommandRequest command_request;
    command_request.set_request(::bosdyn::api::PowerCommandRequest::REQUEST_OFF_PAYLOAD_PORTS);
    return PowerCommand(power_client, command_request, timeout, update_frequency);
}

::bosdyn::common::Status PowerOnWifiRadio(PowerClient* power_client,
                                          ::bosdyn::common::Duration timeout,
                                          double update_frequency) {
    ::bosdyn::api::PowerCommandRequest command_request;
    command_request.set_request(::bosdyn::api::PowerCommandRequest::REQUEST_ON_WIFI_RADIO);
    return PowerCommand(power_client, command_request, timeout, update_frequency);
}

::bosdyn::common::Status PowerOffWifiRadio(PowerClient* power_client,
                                           ::bosdyn::common::Duration timeout,
                                           double update_frequency) {
    ::bosdyn::api::PowerCommandRequest command_request;
    command_request.set_request(::bosdyn::api::PowerCommandRequest::REQUEST_OFF_WIFI_RADIO);
    return PowerCommand(power_client, command_request, timeout, update_frequency);
}



::bosdyn::common::Status PowerCommand(PowerClient* power_client,
                                      ::bosdyn::api::PowerCommandRequest& request,
                                      ::bosdyn::common::Duration timeout, double update_frequency) {
    // The update frequency cannot be 0 because we use it to compute time.
    // NOTE: We are not erroring on negative update_frequency, it triggers no sleep between tries.
    BOSDYN_ASSERT_PRECONDITION(update_frequency != 0.0,
                               "Update frequency for PowerCommand feedback cannot be 0.");

    double start_time = ::bosdyn::common::NsecToSec(::bosdyn::common::NowNsec());
    double end_time = start_time + ::bosdyn::common::NsecToSec(timeout.count());
    double update_time = 1.0 / update_frequency;

    const PowerCommandResultType& result = power_client->PowerCommand(request);
    if (!result) {
        return result.status;
    }

    // Command succeeded immediately.
    if (result.response.status() == ::bosdyn::api::PowerCommandStatus::STATUS_SUCCESS) {
        return ::bosdyn::common::Status(SDKErrorCode::Success);
    }

    int power_command_id = result.response.power_command_id();
    double start_call_time, call_time;
    std::chrono::duration<double> time_until_timeout, sleep_time;
    std::shared_future<PowerCommandFeedbackResultType> fut_feedback;
    PowerCommandFeedbackResultType feedback;
    std::future_status status;
    while (::bosdyn::common::NsecToSec(::bosdyn::common::NowNsec()) < end_time) {
        time_until_timeout = std::chrono::duration<double>(
            end_time - ::bosdyn::common::NsecToSec(::bosdyn::common::NowNsec()));
        start_call_time = ::bosdyn::common::NsecToSec(::bosdyn::common::NowNsec());
        fut_feedback = power_client->PowerCommandFeedbackAsync(power_command_id);
        status = fut_feedback.wait_for(time_until_timeout);
        if (status == std::future_status::ready) {
            feedback = fut_feedback.get();
            if (!feedback) {
                return feedback.status;
            }
            if (feedback.response.status() == ::bosdyn::api::PowerCommandStatus::STATUS_SUCCESS) {
                return ::bosdyn::common::Status(SDKErrorCode::Success);
            }
            if (feedback.response.status() !=
                ::bosdyn::api::PowerCommandStatus::STATUS_IN_PROGRESS) {
                return ::bosdyn::common::Status(feedback.response.status());
            }
        } else if (status == std::future_status::timeout) {
            return ::bosdyn::common::Status(SDKErrorCode::GenericSDKError, "CommandTimedOutError");
        }

        call_time = ::bosdyn::common::NsecToSec(::bosdyn::common::NowNsec()) - start_call_time;
        sleep_time = std::chrono::duration<double>(std::max(0.0, update_time - call_time));
        std::this_thread::sleep_for(sleep_time);
    }

    return ::bosdyn::common::Status(SDKErrorCode::GenericSDKError, "CommandTimedOutError");
}

Result<bool> IsPoweredOn(RobotStateClient* robot_state_client) {
    // Robot state client is required.
    BOSDYN_ASSERT_PRECONDITION(robot_state_client != nullptr,
                               "Robot state client cannot be null for IsPoweredOn.");

    RobotStateResultType result = robot_state_client->GetRobotState();
    if (!result) {
        return {result.status, false};
    }

    if (result.response.robot_state().power_state().motor_power_state() ==
        ::bosdyn::api::PowerState::STATE_ON) {
        return {::bosdyn::common::Status(SDKErrorCode::Success), true};
    }

    return {::bosdyn::common::Status(SDKErrorCode::Success), false};
}

// Fan Power Command Helpers
FanPowerCommandResultType FanPowerCommand(PowerClient* power_client, int percent_power,
                                          double duration) {
    ::bosdyn::api::FanPowerCommandRequest request;
    request.set_percent_power(percent_power);
    ::bosdyn::common::SetDuration((int64_t)(duration * 1e9), request.mutable_duration());

    return power_client->FanPowerCommand(request);
}

FanPowerCommandFeedbackResultType FanPowerCommandFeedback(PowerClient* power_client,
                                                          int command_id) {
    // Simple enough that it could be called directly from PowerClient but want feedback
    // command helper available at same location as fan power command helper

    return power_client->FanPowerCommandFeedback(command_id);
}

}  // namespace power_client_helper

}  // namespace client

}  // namespace bosdyn
