/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


/** The manipulation_api_walk_to example will show to create a robot object, get the E-Stop,
 * timesync endpoint and lease associated with the robot, and then issue commands through the robot
 * command service and manipulation API service.
 *
 * Specifically, this robot will stand up and then walk forward 0.5 meters while doing a raycast
 * from the manipulation API.
 *
 * To run this example, you must set BOSDYN_CLIENT_USERNAME and BOSDYN_CLIENT_PASSWORD environment
 * variables with the robot credentials and then run:
 *     ./manipulation_api_walk_to ROBOT_IP
 */
#include <bosdyn/api/manipulation_api.pb.h>
#include <bosdyn/api/robot_state.pb.h>
#include <CLI/CLI.hpp>
#include "bosdyn/client/lease/lease_client.h"
#include "bosdyn/client/manipulation_api/manipulation_api_client.h"
#include "bosdyn/client/robot_command/robot_command_builder.h"
#include "bosdyn/client/robot_command/robot_command_client.h"
#include "bosdyn/client/robot_state/robot_state_client.h"
#include "bosdyn/client/sdk/client_sdk.h"
#include "bosdyn/client/util/cli_util.h"
#include "bosdyn/math/api_common_frames.h"

::bosdyn::common::Status run(const ::bosdyn::client::CommonCLIArgs& args) {  // Parse the arguments.
    // Create a Client SDK object.
    std::unique_ptr<::bosdyn::client::ClientSdk> client_sdk =
        ::bosdyn::client::CreateStandardSDK("manipulation_api_walk_to");

    // Create a robot instance. A robot instance represents a single user on a single robot.
    ::bosdyn::client::Result<std::unique_ptr<::bosdyn::client::Robot>> robot_result =
        client_sdk->CreateRobot(args.hostname);
    if (!robot_result.status) {
        std::cerr << "Could not create robot: " << robot_result.status.DebugString() << std::endl;
        return robot_result.status;
    }
    std::unique_ptr<::bosdyn::client::Robot> robot = std::move(robot_result.response);

    // Set the username and password to use for authentication with the robot.
    auto status = robot->Authenticate(args.username, args.password);
    if (!status) {
        return status.Chain("Could not authenticate with robot");
        std::cerr << "Could not authenticate with robot: " << status.DebugString() << std::endl;
        return status;
    }
    std::cout << "------Robot instance configured" << std::endl;

    // Create the robot state client.
    ::bosdyn::client::Result<::bosdyn::client::RobotStateClient*> robot_state_client_resp =
        robot->EnsureServiceClient<::bosdyn::client::RobotStateClient>(
            ::bosdyn::client::RobotStateClient::GetDefaultServiceName());
    if (!robot_state_client_resp.status) {
        std::cerr << "Could not create the robot state client: "
                  << robot_state_client_resp.status.DebugString() << std::endl;
        return robot_state_client_resp.status;
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
    if (!robot_cmd_client_resp.status) {
        std::cerr << "Could not create the robot command client: "
                  << robot_cmd_client_resp.status.DebugString() << std::endl;
        return robot_cmd_client_resp.status;
    }
    ::bosdyn::client::RobotCommandClient* robot_command_client = robot_cmd_client_resp.response;

    // Create the manipulation API client.
    ::bosdyn::client::Result<::bosdyn::client::ManipulationApiClient*>
        manipulation_api_client_resp =
            robot->EnsureServiceClient<::bosdyn::client::ManipulationApiClient>(
                ::bosdyn::client::ManipulationApiClient::GetDefaultServiceName());
    if (!manipulation_api_client_resp.status) {
        std::cerr << "Could not create the manipulation API client: "
                  << manipulation_api_client_resp.status.DebugString() << std::endl;
        return manipulation_api_client_resp.status;
    }
    ::bosdyn::client::ManipulationApiClient* manipulation_api_client =
        manipulation_api_client_resp.response;

    // Use the Robot object's timesync endpoint and set a pointer within the robot command client
    // such that it can automatically update command end times in the robot's clock time.
    auto timesync_endpoint_res = robot->StartTimeSyncAndGetEndpoint();
    if (!timesync_endpoint_res.status) {
        std::cerr << "Could not get the robot's timesync endpoint: " << timesync_endpoint_res.status
                  << std::endl;
        return timesync_endpoint_res.status;
    }
    robot_command_client->AddTimeSyncEndpoint(timesync_endpoint_res.response);
    std::cout << "------TimeSync configured" << std::endl;

    // Now acquire a lease. First create a lease client.
    ::bosdyn::client::Result<::bosdyn::client::LeaseClient*> lease_client_resp =
        robot->EnsureServiceClient<::bosdyn::client::LeaseClient>(
            ::bosdyn::client::LeaseClient::GetDefaultServiceName());
    if (!lease_client_resp.status) {
        std::cerr << "Could not create the lease client: " << lease_client_resp.status.DebugString()
                  << std::endl;
        return lease_client_resp.status;
    }
    ::bosdyn::client::LeaseClient* lease_client = lease_client_resp.response;
    // Then acquire the lease for the body.
    auto lease_res = lease_client->AcquireLease(::bosdyn::client::kBodyResource);
    if (!lease_res.status) {
        std::cerr << "Could not acquire the body lease: " << lease_res.status.DebugString()
                  << std::endl;
        return lease_res.status;
    }
    std::cout << "------Lease acquired" << std::endl;

    // Verify the robot is not estopped and that an external application has registered and holds
    // an estop endpoint.
    auto estop_status = robot->IsEstopped();
    if (!estop_status) {
        std::cerr << "Could not check estop status: " << estop_status.status << std::endl;
        return estop_status.status;
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
        std::cerr << "Could not power on the robot: " << power_status.DebugString() << std::endl;
        return power_status;
    }
    std::cout << "------Robot has powered on." << std::endl;

    // Stand up the robot.
    bosdyn::api::RobotCommand stand_command = ::bosdyn::client::StandCommand();
    auto stand_res = robot_command_client->RobotCommand(stand_command);
    if (!stand_res.status) {
        std::cerr << "Failed to complete the stand command: " << stand_res.status.DebugString()
                  << std::endl;
        return stand_res.status;
    }
    std::cout << "------Robot is standing." << std::endl;
    // Wait a few seconds before sending another command.
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Build a walk-to request.
    // This request will walk the robot 0.5 m forward while doing a raycast. If there is something
    // in front of the robot, the command should give the feedback of MANIP_STATE_WALKING_TO_OBJECT
    // while moving, and MANIP_STATE_DONE after the command is complete. If there isn't something
    // in front of the robot, the feedback will be MANIP_STATE_ATTEMPTING_RAYCASTING or
    // MANIP_STATE_GRASP_FAILED_TO_RAYCAST_INTO_MAP until the command completes, and then it will
    // be MANIP_STATE_UNKNOWN.
    ::bosdyn::api::ManipulationApiRequest manip_api_req;

    manip_api_req.mutable_walk_to_object_ray_in_world()->mutable_ray_start_rt_frame()->set_x(0);
    manip_api_req.mutable_walk_to_object_ray_in_world()->mutable_ray_start_rt_frame()->set_y(0);
    manip_api_req.mutable_walk_to_object_ray_in_world()->mutable_ray_start_rt_frame()->set_z(0);

    manip_api_req.mutable_walk_to_object_ray_in_world()->mutable_ray_end_rt_frame()->set_x(0.5);
    manip_api_req.mutable_walk_to_object_ray_in_world()->mutable_ray_end_rt_frame()->set_y(0);
    manip_api_req.mutable_walk_to_object_ray_in_world()->mutable_ray_end_rt_frame()->set_z(0);

    manip_api_req.mutable_walk_to_object_ray_in_world()->set_frame_name(::bosdyn::api::kBodyFrame);

    ::bosdyn::client::ManipulationApiResultType api_req_result =
        manipulation_api_client->ManipulationApi(manip_api_req);
    int cmd_id = api_req_result.response.manipulation_cmd_id();
    ::bosdyn::api::ManipulationFeedbackState current_state =
        ::bosdyn::api::ManipulationFeedbackState::MANIP_STATE_UNKNOWN;
    // Print the ManipulationApiFeedback state for the next 1.5 seconds to see how it updates
    // as our command is running.
    constexpr int max_poll_times = 15;
    for (int i = 0; i < max_poll_times; ++i) {
        ::bosdyn::api::ManipulationApiFeedbackRequest feedback_req;
        feedback_req.set_manipulation_cmd_id(cmd_id);

        ::bosdyn::client::ManipulationApiFeedbackResultType feedback_result =
            manipulation_api_client->ManipulationApiFeedback(feedback_req);

        current_state = feedback_result.response.current_state();
        std::cout << "------Feedback state: "
                  << ::bosdyn::api::ManipulationFeedbackState_Name(current_state) << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Sit down the robot.
    bosdyn::api::RobotCommand poweroff_command = ::bosdyn::client::SafePowerOffCommand();
    auto poweroff_res = robot_command_client->RobotCommand(poweroff_command);
    if (!poweroff_res.status) {
        std::cerr << "Failed to complete the safe power off command: "
                  << poweroff_res.status.DebugString() << std::endl;
        return poweroff_res.status;
    }
    std::cout << "------Robot is powered off." << std::endl;

    return {::bosdyn::client::SDKErrorCode::Success};
}

int main(int argc, char** argv) {
    // Parse the arguments.
    CLI::App parser{"ManipulationAPIWalkTo"};
    ::bosdyn::client::CommonCLIArgs common_args;
    ::bosdyn::client::AddCommonArguments(parser, common_args);
    CLI11_PARSE(parser, argc, argv);

    auto status = run(common_args);
    if (!status) {
        std::cerr << status.DebugString() << std::endl;
        return 1;
    }

    std::cout << "------ManpulationAPIWalkTo application worked correctly." << std::endl;
    return 0;
}
