/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <list>
#include <thread>

#include "bosdyn/client/directory_registration/directory_registration_client.h"
#include "bosdyn/client/processors/request_processor_chain.h"
#include "bosdyn/client/processors/response_processor_chain.h"
#include "bosdyn/client/service_client/message_pump.h"
#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/client/robot/robot.h"

#include <bosdyn/api/directory.pb.h>

#define CHECK_STATUS(s) \
    if (!s) return s

namespace bosdyn {

namespace client {

enum ProxyUseType { USE_PROXY = 0, BYPASS_PROXY = 1, AUTO_DETERMINE = 2 };

// ClientSdk is the root of accessing robots using the Boston Dynamics SDK.
class ClientSdk {
 public:
    ClientSdk();

    ~ClientSdk();

    // Set the client name. Must be called before Init().
    void SetClientName(const std::string& client_name);

    // Load new root certificate for communicating with the robot
    void LoadRobotCert(const std::string& cert) { m_cert = cert; }

    // Load new root certificate for communicating with the robot from a file
    ::bosdyn::common::Status LoadRobotCertFromFile(const std::string& cert_resource_path);

    // Initialize the SDK.
    void Init();

    // Add custom global request processor. This is used across all requests for all robots. Must be
    // called before any robots are created. The processor must be kept alive for the entire
    // lifetime of ClientSdk.
    void AddCustomRequestProcessor(const std::shared_ptr<RequestProcessor>& processor);

    // Add custom global response processor. This is used across all responses for all robots. Must
    // be called before any robots are created. The processor must be kept alive for the entire
    // lifetime of ClientSdk.
    void AddCustomResponseProcessor(const std::shared_ptr<ResponseProcessor>& processor);

    // Return a previously created robot with "robot_name", or create a robot if one does not exist.
    // If proxy_use is set to AUTO_DETERMINE, the network address will be used to determine if
    // proxy should be skipped, meaning cert and token are not specified in the RPCs.
    // Network_address set to "localhost", "127.0.01" and "::1" causes the Clients instantiated
    // from the robot to skip the proxy configuration.
    Result<std::unique_ptr<::bosdyn::client::Robot>> CreateRobot(
        const std::string& network_address,
        const ProxyUseType& proxy_use = AUTO_DETERMINE,
        ::bosdyn::common::Duration timeout = kRPCTimeoutNotSpecified,
        std::shared_ptr<MessagePump> message_pump = nullptr,
        std::shared_ptr<LeaseWallet> lease_wallet = nullptr);

 private:
    void CreateDefaultProcessors();

    bool m_is_initialized = false;
    bool m_is_robot_created = false;

    std::string m_app_token;
    std::string m_client_name;
    std::string m_cert;

    RequestProcessorChain m_request_processor_chain;
    ResponseProcessorChain m_response_processor_chain;

    std::vector<std::shared_ptr<RequestProcessor>> m_default_request_processors;
    std::vector<std::shared_ptr<ResponseProcessor>> m_default_response_processors;
};

// Create a ClientSdk instance and return it as a std::unique_ptr.
std::unique_ptr<ClientSdk> CreateStandardSDK(const std::string& client_name);


}  // namespace client

}  // namespace bosdyn
