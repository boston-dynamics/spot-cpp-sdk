/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "client_sdk.h"

#include <fstream>
#include "bosdyn/client/auth/auth_client.h"
#include "bosdyn/client/auto_return/auto_return_client.h"
#include "bosdyn/client/data_acquisition/data_acquisition_client.h"
#include "bosdyn/client/data_buffer/data_buffer_client.h"
#include "bosdyn/client/directory/directory_client.h"
#include "bosdyn/client/directory_registration/directory_registration_client.h"
#include "bosdyn/client/docking/docking_client.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"
#include "bosdyn/client/estop/estop_client.h"
#include "bosdyn/client/fault/fault_client.h"
#include "bosdyn/client/graph_nav/graph_nav_client.h"
#include "bosdyn/client/graph_nav/recording_client.h"
#include "bosdyn/client/image/image_client.h"
#include "bosdyn/client/lease/lease_client.h"
#include "bosdyn/client/local_grid/local_grid_client.h"
#include "bosdyn/client/log_annotation/log_annotation_client.h"
#include "bosdyn/client/mission/mission_client.h"
#include "bosdyn/client/payload/payload_client.h"
#include "bosdyn/client/payload_registration/payload_registration_client.h"
#include "bosdyn/client/point_cloud/point_cloud_client.h"
#include "bosdyn/client/power/power_client.h"
#include "bosdyn/client/processors/app_token_request_processor.h"
#include "bosdyn/client/processors/common_request_processor.h"
#include "bosdyn/client/processors/common_response_processor.h"
#include "bosdyn/client/robot_command/robot_command_client.h"
#include "bosdyn/client/robot_id/robot_id_client.h"
#include "bosdyn/client/robot_state/robot_state_client.h"
#include "bosdyn/client/time_sync/time_sync_client.h"
#include "bosdyn/client/world_objects/world_object_client.h"

#include "bosdyn/client/spot_cam/audio/audio_client.h"
#include "bosdyn/client/spot_cam/compositor/compositor_client.h"
#include "bosdyn/client/spot_cam/lighting/lighting_client.h"
#include "bosdyn/client/spot_cam/network/network_client.h"
#include "bosdyn/client/spot_cam/power/power_client.h"
#include "bosdyn/client/spot_cam/ptz/ptz_client.h"
#include "bosdyn/client/spot_cam/version/version_client.h"

namespace bosdyn {

namespace client {

std::unique_ptr<ClientSdk> CreateStandardSDK(const std::string& client_name) {
    std::string cert =
        "-----BEGIN CERTIFICATE-----\n"
        "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\n"
        "-----END CERTIFICATE-----";
    std::unique_ptr<ClientSdk> sdk = std::make_unique<ClientSdk>();
    sdk->SetClientName(client_name);
    sdk->LoadRobotCert(cert);
    sdk->Init();
    return sdk;
}


ClientSdk::ClientSdk() = default;

ClientSdk::~ClientSdk() = default;

void ClientSdk::SetClientName(const std::string& client_name) {
    // Must set the client name for the SDK before it is initialized.
    BOSDYN_ASSERT_PRECONDITION(!m_is_initialized, "Client name cannot be set after the SDK is initialized.");
    // Must set the client name for the SDK before any robot instances are created
    // with the SDK.
    BOSDYN_ASSERT_PRECONDITION(!m_is_robot_created, " Client name cannot be set after any robot instances are created.");

    m_client_name = client_name;
}

::bosdyn::common::Status ClientSdk::LoadRobotCertFromFile(const std::string& cert_resource_path) {
    // Reset certificate
    m_cert = "";

    std::ifstream inFile;

    inFile.open(cert_resource_path);
    if (!inFile) {
        return ::bosdyn::common::Status(SDKErrorCode::GenericSDKError, "Unable to open file:" + cert_resource_path);
    }

    std::string line;
    while (std::getline(inFile, line)) {
        m_cert += (line + "\n");
    }

    inFile.close();
    return ::bosdyn::common::Status(SDKErrorCode::Success);
}

void ClientSdk::Init() {
    // Verify that all the information is set correctly
    // SDK cannot be initialized before calling Init().
    BOSDYN_ASSERT_PRECONDITION(!m_is_initialized, "SDK cannot be initialized multiple times.");
    // The robot object cannot be created before calling Init().
    BOSDYN_ASSERT_PRECONDITION(!m_is_robot_created, "SDK cannot be initialized after creating robot objects.");
    // Need to set the client name for the SDK before calling Init().
    BOSDYN_ASSERT_PRECONDITION(!m_client_name.empty(), "SDK cannot be initialized when the client name is empty/unset.");
    CreateDefaultProcessors();
    m_is_initialized = true;
}

void ClientSdk::AddCustomRequestProcessor(const std::shared_ptr<RequestProcessor>& processor) {
    // Verify that all the information is set correctly
    // Must initialize the SDK before calling.
    BOSDYN_ASSERT_PRECONDITION(m_is_initialized, "SDK must be initialized before adding request processors.");
    // Must not have a robot instance created with the SDK before calling.
    BOSDYN_ASSERT_PRECONDITION(!m_is_robot_created, "Request processors cannot be added after creating robot objects.");

    m_request_processor_chain.AppendProcessor(processor);
}

void ClientSdk::AddCustomResponseProcessor(const std::shared_ptr<ResponseProcessor>& processor) {
    // Verify that all the information is set correctly
    // Must initialize the SDK before calling.
    BOSDYN_ASSERT_PRECONDITION(m_is_initialized, "SDK must be initialized before adding processors.");
    // Must not have a robot instance created with the SDK before calling.
    BOSDYN_ASSERT_PRECONDITION(!m_is_robot_created, "Response processors cannot be added after creating robot objects.");

    m_response_processor_chain.AppendProcessor(processor);
}

Result<std::unique_ptr<::bosdyn::client::Robot>> ClientSdk::CreateRobot(
    const std::string& network_address,
    const ProxyUseType& proxy_use, ::bosdyn::common::Duration timeout,
    std::shared_ptr<MessagePump> message_pump,
    std::shared_ptr<LeaseWallet> lease_wallet) {
    // The SDK must be initialized before the robot object can be created.
    BOSDYN_ASSERT_PRECONDITION(m_is_initialized, "SDK must be initialized before creating robot objects.");

    // Check if a port is passed by mistake in the network_address.
    if (network_address.find(':') != std::string::npos &&
        network_address.find("::") == std::string::npos) {
        return {::bosdyn::common::Status(SDKErrorCode::GenericSDKError,
                       "ERROR, network address contains :, but string is supposed to contain"
                       "only network address without a port"),
                nullptr};
    }

    bool bypass_proxy = false;
    if (proxy_use == BYPASS_PROXY) {
        bypass_proxy = true;
    } else if (proxy_use == USE_PROXY) {
        bypass_proxy = false;
    } else {  // proxy_use is set to AUTO_DETERMINE
        if (network_address.compare("127.0.0.1") == 0 || network_address.compare("::1") == 0 ||
            network_address.compare("localhost") == 0) {
            bypass_proxy = true;
        }
    }

    std::unique_ptr<Robot> robot = std::make_unique<Robot>(m_client_name, bypass_proxy, timeout);
    robot->SetNetworkAddress(network_address);
    robot->SetGlobalProcessorChains(m_request_processor_chain, m_response_processor_chain);

    if (!bypass_proxy) {
        robot->SetRobotCert(m_cert);
    }

    if (!message_pump) {
        // ClientSdk uses one message pump per Robot by default.
        message_pump = std::make_shared<MessagePump>();
        message_pump->AutoUpdate(std::chrono::milliseconds(100));
    }
    robot->SetDefaultMessagePump(message_pump);
    if (lease_wallet) robot->SetWallet(lease_wallet);

    m_is_robot_created = true;

    return {::bosdyn::common::Status(SDKErrorCode::Success), std::move(robot)};
}

void ClientSdk::CreateDefaultProcessors() {
    // Create RequestProcessor's whose lifetime is owned by ClientSdk.
    m_default_request_processors.push_back(std::make_unique<CommonRequestProcessor>(m_client_name));
    // m_default_request_processors.push_back(std::make_unique<AppTokenRequestProcessor>(m_app_token));

    // Create ResponseProcessor's whose lifetime is owned by ClientSdk.
    m_default_response_processors.push_back(std::make_unique<CommonResponseProcessor>());

    // Populate Request Processor Chain with defaults.
    for (const auto& request_processor : m_default_request_processors) {
        m_request_processor_chain.AppendProcessor(request_processor);
    }

    // Populate Response Processor Chain with defaults.
    for (const auto& response_processor : m_default_response_processors) {
        m_response_processor_chain.AppendProcessor(response_processor);
    }
}

}  // namespace client

}  // namespace bosdyn
