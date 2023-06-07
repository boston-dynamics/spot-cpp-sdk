/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


/** The arm_grasp_carry_override example will show to create a robot object, get the E-Stop,
 * timesync endpoint and lease associated with the robot, and then issue commands through the robot
 * command service and manipulation API service.
 *
 * Specifically, this robot will:
 * Stand the robot up and bring the arm to READY.
 * Print the current grasp and carry override states.
 * Set the grasp state to HOLDING and carry state to NOT_CARRIABLE.
 * Print the current grasp and carry override states.
 * Confirm it is not possible to stow the arm.
 * Set the grasp state to HOLDING and carry state to CARRIABLE.
 * Print the current grasp and carry override states.
 * Confirm it is not possible to stow the arm.
 * Set the grasp state to HOLDING and carry state to CARRIABLE_AND_STOWABLE.
 * Print the current grasp and carry override states.
 * Confirm it is possible to stow the arm.
 * Bring the arm back to READY.
 * Set the grasp state to HOLDING.
 * Open the gripper.
 * Print the current grasp and carry override states, and confirm the grasp state is NOT_HOLDING.
 * Close the gripper.
 * Print the current grasp and carry override states, and confirm the grasp state is NOT_HOLDING.
 * Set the grasp state back to HOLDING.
 * Print the current grasp and carry override states.
 * Set the grasp state to NOT_HOLDING.
 * Print the current grasp and carry override states.
 * Confirm it is possible to stow the arm.
 * Power off the robot.
 *
 * To run this example, you must set BOSDYN_CLIENT_USERNAME and BOSDYN_CLIENT_PASSWORD environment
 * variables with the robot credentials and then run:
 *     ./arm_grasp_carry_override ROBOT_IP
 */

#include <bosdyn/api/manipulation_api.pb.h>
#include <bosdyn/api/robot_state.pb.h>
#include <CLI/CLI.hpp>
#include "bosdyn/client/lease/lease_client.h"
#include "bosdyn/client/manipulation_api/manipulation_api_client.h"
#include "bosdyn/client/robot_command/robot_command_builder.h"
#include "bosdyn/client/robot_command/robot_command_client.h"
#include "bosdyn/client/robot_command/robot_command_helpers.h"
#include "bosdyn/client/robot_state/robot_state_client.h"
#include "bosdyn/client/sdk/client_sdk.h"
#include "bosdyn/client/util/cli_util.h"

void PrintGripperHoldingAndCarryState(const ::bosdyn::client::RobotStateResultType& result) {
    std::cout << "------Gripper Holding Something? " << std::boolalpha
              << result.response.robot_state().manipulator_state().is_gripper_holding_item()
              << std::endl;

    std::cout << "------Arm carry state: "
              << ::bosdyn::api::ManipulatorState_CarryState_Name(
                     result.response.robot_state().manipulator_state().carry_state())
              << std::endl;
}

void WaitForUserInput() {
    std::cout << "Press Enter to Continue";
    std::cin.ignore();
}

void WaitUntilGraspStateUpdates(
    const ::bosdyn::api::ApiGraspOverrideRequest& grasp_override_request,
    ::bosdyn::client::RobotStateClient* robot_state_client) {
    bool have_overrides_updated = false;
    bool has_grasp_override = grasp_override_request.has_api_grasp_override();
    bool has_carry_override = grasp_override_request.has_carry_state_override();
    using namespace std::chrono_literals;
    auto poll_period = 100ms;
    while (!have_overrides_updated) {
        auto robot_state = robot_state_client->GetRobotState().response.robot_state();
        bool grasp_state_updated =
            (robot_state.manipulator_state().is_gripper_holding_item() &&
             grasp_override_request.api_grasp_override().override_request() ==
                 ::bosdyn::api::ApiGraspOverride::OVERRIDE_HOLDING) ||
            (!robot_state.manipulator_state().is_gripper_holding_item() &&
             grasp_override_request.api_grasp_override().override_request() ==
                 ::bosdyn::api::ApiGraspOverride::OVERRIDE_NOT_HOLDING);

        bool carry_state_updated =
            has_carry_override &&
            (robot_state.manipulator_state().carry_state() ==
             grasp_override_request.carry_state_override().override_request());

        have_overrides_updated = (!has_grasp_override || grasp_state_updated) &&
                                 (!has_carry_override || carry_state_updated);
        std::this_thread::sleep_for(poll_period);
    }
}

::bosdyn::common::Status run(const ::bosdyn::client::CommonCLIArgs& args) {  // Parse the arguments.
    // Create a Client SDK object.
    std::unique_ptr<::bosdyn::client::ClientSdk> client_sdk =
        ::bosdyn::client::CreateStandardSDK("arm_grasp_carry_override");

    // Create a robot instance. A robot instance represents a single user on a single robot.
    ::bosdyn::client::Result<std::unique_ptr<::bosdyn::client::Robot>> robot_result =
        client_sdk->CreateRobot(args.hostname);
    if (!robot_result) {
        return robot_result.status.Chain("Could not create robot");
    }
    std::unique_ptr<::bosdyn::client::Robot> robot = std::move(robot_result.response);

    // Set the username and password to use for authentication with the robot.
    auto status = robot->Authenticate(args.username, args.password);
    if (!status) {
        return status.Chain("Could not authenticate with robot");
    }
    std::cout << "------Robot instance configured" << std::endl;

    // Create the robot state client.
    ::bosdyn::client::Result<::bosdyn::client::RobotStateClient*> robot_state_client_resp =
        robot->EnsureServiceClient<::bosdyn::client::RobotStateClient>(
            ::bosdyn::client::RobotStateClient::GetDefaultServiceName());
    if (!robot_state_client_resp) {
        return robot_state_client_resp.status.Chain("Could not create the robot state client");
    }
    ::bosdyn::client::RobotStateClient* robot_state_client = robot_state_client_resp.response;

    // Make sure the robot has an arm.
    if (!robot_state_client->GetRobotState().response.robot_state().has_manipulator_state()) {
        std::cerr << "Robot must have an arm to run this example" << std::endl;
        return {::bosdyn::client::SDKErrorCode::GenericSDKError};
    }

    // Create the robot command client.
    ::bosdyn::client::Result<::bosdyn::client::RobotCommandClient*> robot_cmd_client_resp =
        robot->EnsureServiceClient<::bosdyn::client::RobotCommandClient>(
            ::bosdyn::client::RobotCommandClient::GetDefaultServiceName());
    if (!robot_cmd_client_resp) {
        return robot_cmd_client_resp.status.Chain("Could not create the robot command client");
    }
    ::bosdyn::client::RobotCommandClient* robot_command_client = robot_cmd_client_resp.response;

    // Create the manipulation API client.
    ::bosdyn::client::Result<::bosdyn::client::ManipulationApiClient*>
        manipulation_api_client_resp =
            robot->EnsureServiceClient<::bosdyn::client::ManipulationApiClient>(
                ::bosdyn::client::ManipulationApiClient::GetDefaultServiceName());
    if (!manipulation_api_client_resp) {
        return manipulation_api_client_resp.status.Chain(
            "Could not create the manipulation API client");
    }
    ::bosdyn::client::ManipulationApiClient* manipulation_api_client =
        manipulation_api_client_resp.response;

    // Use the Robot object's timesync endpoint and set a pointer within the robot command client
    // such that it can automatically update command end times in the robot's clock time.
    auto timesync_endpoint_res = robot->StartTimeSyncAndGetEndpoint();
    if (!timesync_endpoint_res) {
        return timesync_endpoint_res.status.Chain("Could not get the robot's timesync endpoint");
    }
    robot_command_client->AddTimeSyncEndpoint(timesync_endpoint_res.response);
    std::cout << "------TimeSync configured" << std::endl;

    // Now acquire a lease. First create a lease client.
    ::bosdyn::client::Result<::bosdyn::client::LeaseClient*> lease_client_resp =
        robot->EnsureServiceClient<::bosdyn::client::LeaseClient>(
            ::bosdyn::client::LeaseClient::GetDefaultServiceName());
    if (!lease_client_resp) {
        return lease_client_resp.status.Chain("Could not create the lease client");
    }
    ::bosdyn::client::LeaseClient* lease_client = lease_client_resp.response;
    // Then acquire the lease for the body.
    auto lease_res = lease_client->AcquireLease("body");
    if (!lease_res) {
        return lease_res.status.Chain("Could not acquire the body lease");
    }
    std::cout << "------Lease acquired" << std::endl;

    // Verify the robot is not estopped and that an external application has registered and holds
    // an estop endpoint.
    auto estop_status = robot->IsEstopped();
    if (!estop_status) {
        return estop_status.status.Chain("Could not check estop status");
    }
    if (estop_status.response) {
        std::cerr << "Robot is estopped. Please use an external E-Stop client, such as the estop"
                     "Python SDK example, to configure E-Stop."
                  << std::endl;
        return {::bosdyn::client::SDKErrorCode::GenericSDKError};
    }
    std::cout << "------E-Stop configured" << std::endl;

    // Power on the robot.
    auto power_status = robot->PowerOnMotors(std::chrono::seconds(60), 1.0);
    if (!power_status) {
        return power_status.Chain("Could not power on the robot");
    }
    std::cout << "------Robot has powered on." << std::endl;

    // Stand up the robot.
    bosdyn::api::RobotCommand stand_command = ::bosdyn::client::StandCommand();
    auto stand_res = robot_command_client->RobotCommand(stand_command);
    if (!stand_res) {
        return stand_res.status.Chain("Failed to complete the stand command");
    }
    std::cout << "------Robot is standing." << std::endl;

    WaitForUserInput();
    std::cout << "\nCommanding the arm to ready." << std::endl;
    ::bosdyn::api::RobotCommand ready_command = ::bosdyn::client::ArmReadyCommand();
    auto ready_res = robot_command_client->RobotCommand(ready_command);
    if (!ready_res) {
        return ready_res.status.Chain("Failed to send the arm ready command");
    }
    constexpr int move_timeout_sec = 5;
    auto block_res = BlockUntilArmArrives(*robot_command_client,
                                          ready_res.response.robot_command_id(), move_timeout_sec);
    if (!block_res) {
        return block_res.status.Chain("Ready command failed");
    }
    std::cout << "------Arm is readied." << std::endl;

    // Expected output:
    // Gripper holding something? false
    // Arm carry state: [Can be anything depending on the previous setting.]
    PrintGripperHoldingAndCarryState(robot_state_client->GetRobotState());

    ///////////////// HOLDING, NOT CARRIABLE /////////////////
    WaitForUserInput();
    std::cout << "\nSetting grasp override to HOLDING and carry override to NOT_CARRIABLE."
              << std::endl;
    // Combine separate grasp and carry state override requests into a single request to send to
    // the robot over the manipulation API.
    ::bosdyn::api::ApiGraspOverrideRequest holding_not_carriable_override_request;
    holding_not_carriable_override_request.mutable_api_grasp_override()->set_override_request(
        ::bosdyn::api::ApiGraspOverride::OVERRIDE_HOLDING);
    holding_not_carriable_override_request.mutable_carry_state_override()->set_override_request(
        ::bosdyn::api::ManipulatorState::CARRY_STATE_NOT_CARRIABLE);

    ::bosdyn::client::OverrideGraspResultType ogrt =
        manipulation_api_client->OverrideGrasp(holding_not_carriable_override_request);
    WaitUntilGraspStateUpdates(holding_not_carriable_override_request, robot_state_client);
    // Expected output:
    // Gripper holding something? true
    // Arm carry state: NOT_CARRIABLE
    PrintGripperHoldingAndCarryState(robot_state_client->GetRobotState());

    std::cout << "------Commanding arm to stow, but it will fail." << std::endl;
    ::bosdyn::api::RobotCommand stow_command = ::bosdyn::client::ArmStowCommand();
    auto stow_res = robot_command_client->RobotCommand(stow_command);
    if (!stow_res) {
        return stow_res.status.Chain("Failed to send the arm stow command");
    }
    block_res = BlockUntilArmArrives(*robot_command_client, stow_res.response.robot_command_id(),
                                     move_timeout_sec);
    if (block_res) {
        std::cerr << "The arm should not stow when holding something that is not carriable. "
                  << block_res.status.DebugString() << std::endl;
        return {::bosdyn::client::SDKErrorCode::GenericSDKError};
    }
    std::cout << "------Arm did not stow." << std::endl;

    ///////////////// HOLDING, CARRIABLE /////////////////
    WaitForUserInput();
    std::cout << "\nSetting grasp override to HOLDING and carry override to CARRIABLE."
              << std::endl;
    // Combine separate grasp and carry state override requests into a single request to send to
    // the robot over the manipulation API.
    ::bosdyn::api::ApiGraspOverrideRequest holding_carriable_override_request;
    holding_carriable_override_request.mutable_api_grasp_override()->set_override_request(
        ::bosdyn::api::ApiGraspOverride::OVERRIDE_HOLDING);
    holding_carriable_override_request.mutable_carry_state_override()->set_override_request(
        ::bosdyn::api::ManipulatorState::CARRY_STATE_CARRIABLE);
    ogrt = manipulation_api_client->OverrideGrasp(holding_carriable_override_request);
    WaitUntilGraspStateUpdates(holding_carriable_override_request, robot_state_client);

    // Expected output:
    // Gripper holding something? true
    // Arm carry state: CARRIABLE
    PrintGripperHoldingAndCarryState(robot_state_client->GetRobotState());

    std::cout << "------Commanding arm to stow, but it will fail." << std::endl;
    stow_res = robot_command_client->RobotCommand(stow_command);
    if (!stow_res) {
        return stow_res.status.Chain("Failed to send the arm stow command");
    }
    block_res = BlockUntilArmArrives(*robot_command_client, stow_res.response.robot_command_id(),
                                     move_timeout_sec);
    if (block_res) {
        std::cerr
            << "The arm should not stow when holding something that is carriable but not stowable."
            << std::endl;
        return {::bosdyn::client::SDKErrorCode::GenericSDKError};
    }
    std::cout << "------Arm did not stow." << std::endl;

    ///////////////// HOLDING, CARRIABLE_AND_STOWABLE /////////////////
    WaitForUserInput();
    std::cout << "\nSetting grasp override to HOLDING and carry override to CARRIABLE_AND_STOWABLE."
              << std::endl;
    ::bosdyn::api::ApiGraspOverrideRequest holding_carriable_and_stowable_override_request;
    holding_carriable_and_stowable_override_request.mutable_api_grasp_override()
        ->set_override_request(::bosdyn::api::ApiGraspOverride::OVERRIDE_HOLDING);
    holding_carriable_and_stowable_override_request.mutable_carry_state_override()
        ->set_override_request(::bosdyn::api::ManipulatorState::CARRY_STATE_CARRIABLE_AND_STOWABLE);
    ogrt = manipulation_api_client->OverrideGrasp(holding_carriable_and_stowable_override_request);
    WaitUntilGraspStateUpdates(holding_carriable_and_stowable_override_request, robot_state_client);
    // Expected output:
    // Gripper holding something? true
    // Arm carry state: CARRIABLE_AND_STOWABLE
    PrintGripperHoldingAndCarryState(robot_state_client->GetRobotState());

    std::cout << "------Commanding arm to stow, and it will succeed." << std::endl;
    stow_res = robot_command_client->RobotCommand(stow_command);
    if (!stow_res) {
        return stow_res.status.Chain("Failed to send the arm stow command");
    }
    block_res = BlockUntilArmArrives(*robot_command_client, stow_res.response.robot_command_id(),
                                     move_timeout_sec);
    if (!block_res) {
        return block_res.status.Chain(
            "The arm should stow when holding something that is carriable and stowable");
    }
    std::cout << "------Arm successfully stowed." << std::endl;

    WaitForUserInput();
    std::cout << "\nCommanding the arm back to ready." << std::endl;
    ready_res = robot_command_client->RobotCommand(ready_command);
    if (!ready_res) {
        return ready_res.status.Chain("Failed to send the arm ready command");
    }
    block_res = BlockUntilArmArrives(*robot_command_client, ready_res.response.robot_command_id(),
                                     move_timeout_sec);
    if (!block_res) {
        return block_res.status.Chain("The arm should move when holding something");
    }
    std::cout << "------Arm is readied." << std::endl;

    ///////////////// Clear the HOLDING state by opening the gripper /////////////////
    WaitForUserInput();
    std::cout << "\nSetting grasp override to HOLDING." << std::endl;
    ::bosdyn::api::ApiGraspOverrideRequest holding_override_request;
    holding_override_request.mutable_api_grasp_override()->set_override_request(
        ::bosdyn::api::ApiGraspOverride::OVERRIDE_HOLDING);
    ogrt = manipulation_api_client->OverrideGrasp(holding_override_request);
    WaitUntilGraspStateUpdates(holding_override_request, robot_state_client);
    // Expected output:
    // Gripper holding something? true
    // Arm carry state: CARRIABLE_AND_STOWABLE [This is from our last override.]
    PrintGripperHoldingAndCarryState(robot_state_client->GetRobotState());

    std::cout << "\nOpening the gripper to clear the grasp state." << std::endl;
    ::bosdyn::api::RobotCommand open_gripper_command = ::bosdyn::client::ClawGripperOpenCommand();
    auto open_res = robot_command_client->RobotCommand(open_gripper_command);
    if (!open_res) {
        return open_res.status.Chain("Failed to send open gripper command");
    }

    block_res = BlockUntilGripperArrives(*robot_command_client,
                                         open_res.response.robot_command_id(), move_timeout_sec);
    if (!block_res) {
        return block_res.status.Chain("The gripper should open when holding an object");
    }
    std::cout << "------Gripper is open." << std::endl;

    // Expected output:
    // Gripper holding something? false
    // Arm carry state: CARRY_STATE_CARRIABLE_AND_STOWABLE [This is from our last override.]
    PrintGripperHoldingAndCarryState(robot_state_client->GetRobotState());

    WaitForUserInput();
    std::cout << "\nClosing the gripper, which will not clear the grasp state." << std::endl;
    ::bosdyn::api::RobotCommand close_gripper_command = ::bosdyn::client::ClawGripperCloseCommand();
    auto close_res = robot_command_client->RobotCommand(close_gripper_command);
    if (!close_res) {
        return close_res.status.Chain("Failed to send close gripper command");
    }
    block_res = BlockUntilGripperArrives(*robot_command_client,
                                         close_res.response.robot_command_id(), move_timeout_sec);
    if (!block_res) {
        return block_res.status.Chain("The gripper should close when not holding an object");
    }
    std::cout << "------Gripper is closed." << std::endl;

    // Expected output:
    // Gripper holding something? false
    // Arm carry state: CARRY_STATE_CARRIABLE_AND_STOWABLE [This is from our last override.]
    PrintGripperHoldingAndCarryState(robot_state_client->GetRobotState());

    std::cout << "\nSetting grasp override back to HOLDING." << std::endl;
    ogrt = manipulation_api_client->OverrideGrasp(holding_override_request);
    WaitUntilGraspStateUpdates(holding_override_request, robot_state_client);
    // Expected output:
    // Gripper holding something? true
    // Arm carry state: CARRY_STATE_NOT_CARRIABLE [This is because we didn't request any state.]
    PrintGripperHoldingAndCarryState(robot_state_client->GetRobotState());

    ///////////////// NOT_HOLDING /////////////////
    std::cout << "\nSetting grasp override to NOT_HOLDING." << std::endl;
    ::bosdyn::api::ApiGraspOverrideRequest not_holding_override_request;
    not_holding_override_request.mutable_api_grasp_override()->set_override_request(
        ::bosdyn::api::ApiGraspOverride::OVERRIDE_NOT_HOLDING);
    ogrt = manipulation_api_client->OverrideGrasp(not_holding_override_request);
    WaitUntilGraspStateUpdates(not_holding_override_request, robot_state_client);
    // Expected output:
    // Gripper holding something? false
    // Arm carry state: CARRY_STATE_NOT_CARRIABLE [This is from our last override.]
    PrintGripperHoldingAndCarryState(robot_state_client->GetRobotState());

    // Sit down the robot.
    bosdyn::api::RobotCommand poweroff_command = ::bosdyn::client::SafePowerOffCommand();
    auto poweroff_res = robot_command_client->RobotCommand(poweroff_command);
    if (!poweroff_res) {
        return poweroff_res.status.Chain("Failed to complete the safe power off command");
    }
    std::cout << "------Robot is powered off." << std::endl;

    return {::bosdyn::client::SDKErrorCode::Success};
}

int main(int argc, char** argv) {
    // Parse the arguments.
    CLI::App parser{"ArmGraspCarryOverrides"};
    ::bosdyn::client::CommonCLIArgs common_args;
    ::bosdyn::client::AddCommonArguments(parser, common_args);
    CLI11_PARSE(parser, argc, argv);

    auto status = run(common_args);
    if (!status) {
        std::cerr << status.DebugString() << std::endl;
        return 1;
    }

    std::cout << "------ArmGraspCarryOverrides application worked correctly." << std::endl;
    return 0;
}
