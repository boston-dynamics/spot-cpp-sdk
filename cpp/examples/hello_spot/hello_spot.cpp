/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include <CLI/CLI.hpp>

#include "bosdyn/client/sdk/client_sdk.h"
#include "bosdyn/client/data_buffer/data_buffer_client.h"
#include "bosdyn/client/directory/directory_client.h"
#include "bosdyn/client/payload/payload_client.h"
#include "bosdyn/client/robot_id/robot_id_client.h"
#include "bosdyn/client/robot_state/robot_state_client.h"
#include "bosdyn/client/time_sync/time_sync_helpers.h"
#include "bosdyn/client/util/cli_util.h"


// Show how to add additional processor in the SDK that logs requests and responses to standard error.
class StderrProcessor : public ::bosdyn::client::RequestProcessor, public ::bosdyn::client::ResponseProcessor {
 public:
    StderrProcessor() = default;
    ~StderrProcessor() override = default;

    // RequestProcessor
    ::bosdyn::common::Status Process(grpc::ClientContext*, bosdyn::api::RequestHeader*,
                           google::protobuf::Message* full_request) override {
        std::cerr << "Request: " << full_request->DebugString() << std::endl;
        return {::bosdyn::client::SDKErrorCode::Success};
    }

    // ResponseProcessor
    ::bosdyn::common::Status Process(const grpc::Status& status, const bosdyn::api::ResponseHeader&,
                           const google::protobuf::Message& full_response) override {
        std::cerr << "status: " << status.error_code() << std::endl;
        std::cerr << "response: " << full_response.DebugString() << std::endl;
        return {::bosdyn::client::SDKErrorCode::Success};
    }
};

::bosdyn::common::Status GetRobotId(::bosdyn::client::Robot* robot) {
    // Create a "RobotIdClient".
    auto robot_id_client_result = robot->EnsureServiceClient<::bosdyn::client::RobotIdClient>();
    if (!robot_id_client_result) {
        return robot_id_client_result.status.Chain("Could not create robot id client");
    }

    // Synchronously retrieve the robot id result and print it out.
    auto result = robot_id_client_result.response->Get();
    if (!result) {
        std::cerr << "RobotId Error: " << result.status.DebugString() << std::endl;
        return result.status;
    }

    std::cout << "RobotId result: " << result.response.DebugString() << std::endl;
    return {::bosdyn::client::SDKErrorCode::Success};
}


::bosdyn::common::Status GetServiceInformation(::bosdyn::client::Robot* robot) {
    // Synchronously retrieve the list of services running on the robot and print them out.
    auto services_result = robot->ListServices();
    if (!services_result) {
        return services_result.status.Chain("Robot Directory error");
    }
    for (const auto& service : services_result.response)
        std::cout << service.DebugString() << std::endl;

    // Create a DirectoryClient.
    auto dir_client_result = robot->EnsureServiceClient<::bosdyn::client::DirectoryClient>();
    if (!dir_client_result) {
        return dir_client_result.status.Chain("Could not create directory client");
    }

    // Asynchronously retrieve information for three services running on the robot: Payload,
    // DataBuffer and RobotState.
    auto fut_payload = dir_client_result.response->GetServiceEntryAsync(
        ::bosdyn::client::PayloadClient::GetDefaultServiceName());
    auto fut_data_buffer = dir_client_result.response->GetServiceEntryAsync(
        ::bosdyn::client::DataBufferClient::GetDefaultServiceName());
    auto fut_state = dir_client_result.response->GetServiceEntryAsync(
        ::bosdyn::client::RobotStateClient::GetDefaultServiceName());

    // Print out RobotState service information.
    ::bosdyn::client::DirectoryEntryResultType fut_result = fut_state.get();
    if (!fut_result) {
        std::cerr << "Could not get state service info " << fut_result.status.DebugString()
                  << std::endl;
        return fut_result.status;
    }
    std::cout << "---STATE---:\n" << fut_result.response.DebugString() << std::endl;

    // Print out Payload service information.
    fut_result = fut_payload.get();
    if (!fut_result) {
        std::cerr << "Could not get payload service info " << fut_result.status.DebugString()
                  << std::endl;
        return fut_result.status;
    }
    std::cout << "---PAYLOAD---:\n" << fut_result.response.DebugString() << std::endl;

    // Print out DataBuffer service information.
    fut_result = fut_data_buffer.get();
    if (!fut_result) {
        std::cerr << "Could not get data buffer service info " << fut_result.status.DebugString()
                  << std::endl;
        return fut_result.status;
    }
    std::cout << "---DATA BUFFER---:\n" << fut_result.response.DebugString() << std::endl;

    return {::bosdyn::client::SDKErrorCode::Success};
}


::bosdyn::common::Status LogToDataBuffer(::bosdyn::client::Robot* robot) {
    // Create a DataBufferClient.
    auto log_client_result = robot->EnsureServiceClient<::bosdyn::client::DataBufferClient>();
    if (!log_client_result) {
        return log_client_result.status.Chain("Could not create data buffer client");
    }

    // Synchronously log the RobotIdResponse protobuf message.
    auto id_result = robot->GetId();
    if (!id_result) {
        return id_result.status.Chain("Robot Id error");
    }
    const auto& proto_to_log = id_result.response;

    bosdyn::api::DataBlob blob;
    *blob.mutable_timestamp() = proto_to_log.header().response_timestamp();
    blob.set_channel(proto_to_log.descriptor()->full_name());
    blob.set_type_id(proto_to_log.descriptor()->full_name());
    proto_to_log.SerializeToString(blob.mutable_data());
    ::bosdyn::client::RecordDataBlobsResultType log_result =
        log_client_result.response->RecordDataBlob(blob);
    if (!log_result) {
        std::cerr << "Log Error: " << log_result.status.DebugString() << std::endl;
        return log_result.status;
    }

    std::cout << "Log result: " << log_result.response.DebugString() << std::endl;
    return {::bosdyn::client::SDKErrorCode::Success};
}


::bosdyn::common::Status GetPayloadInformation(::bosdyn::client::Robot* robot) {
    // Create a PayloadClient.
    auto payload_client_result = robot->EnsureServiceClient<::bosdyn::client::PayloadClient>();
    if (!payload_client_result) {
        return payload_client_result.status.Chain("Could not create payload client");
    }

    // Synchronously get the list of payloads.
    ::bosdyn::client::ListPayloadsResultType payloads_result = payload_client_result.response->ListPayloads();
    if (!payloads_result) {
        std::cerr << "Could not get payload information " << payloads_result.status.DebugString()
                  << std::endl;
        return payloads_result.status;
    }

    std::cout << "---Payloads---:\n" << payloads_result.response.DebugString() << std::endl;
    return {::bosdyn::client::SDKErrorCode::Success};
}


::bosdyn::common::Status GetRobotStateInformation(::bosdyn::client::Robot* robot) {
    // Get the current robot state, hardware config and metrics.
    auto robot_state_client_resp = robot->EnsureServiceClient<::bosdyn::client::RobotStateClient>();
    if (!robot_state_client_resp) {
        return robot_state_client_resp.status.Chain("Could not create the robot state client");
    }
    auto robot_state_client = robot_state_client_resp.move();
    // Get the full robot state, including kinematics state, battery/estop states, and
    // system/behavior faults.
    ::bosdyn::client::RobotStateResultType robot_state = robot_state_client->GetRobotState();
    if (!robot_state) {
        std::cerr << "Could not get the robot state information: "
                  << robot_state.status.DebugString() << std::endl;
        return robot_state.status;
    }
    std::cout << "Robot State: " << robot_state.response.DebugString() << std::endl;

    // Get the robot state metrics.
    ::bosdyn::client::RobotMetricsResultType robot_metrics = robot_state_client->GetRobotMetrics();
    if (!robot_metrics) {
        std::cerr << "Could not get the robot metrics information: "
                  << robot_metrics.status.DebugString() << std::endl;
        return robot_metrics.status;
    }
    std::cout << "Robot Metrics: " << robot_metrics.response.DebugString() << std::endl;

    // Get the hardware configuration for the robot.
    ::bosdyn::client::HardwareConfigurationResultType hardware_config =
        robot_state_client->GetRobotHardwareConfiguration();
    if (!hardware_config) {
        std::cerr << "Could not get the hardware configuration information: "
                  << hardware_config.status.DebugString() << std::endl;
        return hardware_config.status;
    }
    std::cout << "Robot hardware configuration: " << hardware_config.response.DebugString()
              << std::endl;

    return {::bosdyn::client::SDKErrorCode::Success};
}


::bosdyn::common::Status EstablishTimeSync(::bosdyn::client::Robot* robot) {
    // Establish time synchronization with the robot.
    auto time_sync_client_resp = robot->EnsureServiceClient<::bosdyn::client::TimeSyncClient>();
    if (!time_sync_client_resp) {
        return time_sync_client_resp.status.Chain("Could not create the time sync client");
    }
    ::bosdyn::client::TimeSyncThread time_sync_thread(time_sync_client_resp.move());
    if (time_sync_thread.HasEstablishedTimeSync()) {
        return {::bosdyn::client::SDKErrorCode::GenericSDKError, "Faulty establishment of time sync."};
    }
    time_sync_thread.Start();
    if (!time_sync_thread.WaitForSync(std::chrono::seconds(5))) {
        time_sync_thread.Stop();
        return {::bosdyn::client::SDKErrorCode::GenericSDKError, "Failed to establish time sync before timing out."};
    }
    time_sync_thread.Stop();

    return {::bosdyn::client::SDKErrorCode::Success};
}


::bosdyn::common::Status run(const ::bosdyn::client::CommonCLIArgs& args) {
    // Create a Client SDK object.
    std::unique_ptr<::bosdyn::client::ClientSdk> client_sdk = ::bosdyn::client::CreateStandardSDK("hello_spot");
    std::cout << "------Created SDK" << std::endl;

    // Create a debugging processor. Clients can add custom request and response processors which
    // get invoked on every request.
    std::shared_ptr<StderrProcessor> stderr_processor = std::make_shared<StderrProcessor>();
    client_sdk->AddCustomRequestProcessor(stderr_processor);
    client_sdk->AddCustomResponseProcessor(stderr_processor);

    // Create a robot instance. A robot instance represents a single user on a single robot.
    auto robot_result = client_sdk->CreateRobot(args.hostname);
    if (!robot_result) {
        return robot_result.status.Chain("Could not create robot");
    }
    auto robot = robot_result.move();
    std::cout << "------Created Robot" << std::endl;

    // Set the username and password to use. If you do not already have an account on a robot, ask
    // your administrator to add an account.
    ::bosdyn::common::Status status = robot->Authenticate(args.username, args.password);
    if (!status) {
        return status.Chain("Could not authenticate with robot");
    }
    std::cout << "------Authenticated with Robot" << std::endl;

    // Get robot id.
    status = GetRobotId(robot.get());
    if (!status) {
        return status;
    }

    // Get various service information from the DirectoryClient.
    status = GetServiceInformation(robot.get());
    if (!status) {
        return status;
    }

    // Log protobuf message to DataBufferClient.
    status = LogToDataBuffer(robot.get());
    if (!status) {
        return status;
    }

    // Get payload information.
    status = GetPayloadInformation(robot.get());
    if (!status) {
        return status;
    }

    // Get robot state information.
    status = GetRobotStateInformation(robot.get());
    if (!status) {
        return status;
    }

    // Establish time synchronization with the robot. This is needed for sending robot commands to
    // the robot.
    status = EstablishTimeSync(robot.get());
    if (!status) {
        return status;
    }

    return {::bosdyn::client::SDKErrorCode::Success};
}


int main(int argc, char** argv) {

    // Parse the arguments.
    CLI::App parser{"HelloSpot"};
    ::bosdyn::client::CommonCLIArgs common_args;
    ::bosdyn::client::AddCommonArguments(parser, common_args);
    CLI11_PARSE(parser, argc, argv);

    auto status = run(common_args);
    if (!status) {
        std::cerr << status.DebugString() << std::endl;
        return 1;
    }

    std::cout << "------HelloSpot application worked correctly." << std::endl;
    return 0;
}
