/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/network_compute_bridge.pb.h>
#include <bosdyn/api/network_compute_bridge_service.grpc.pb.h>

#include <future>

#include "network_compute_bridge_error_codes.h"
#include "bosdyn/client/service_client/service_client.h"

namespace bosdyn {

namespace client {

// Defining result types for each RPC featuring the RPC call's status and the response message
// the RPC was returning from the robot
typedef Result<::bosdyn::api::NetworkComputeResponse> NetworkComputeResultType;
typedef Result<::bosdyn::api::ListAvailableModelsResponse> ListAvailableModelsResultType;

// Client which communicates with the NetworkComputeBridgeWorker service
class NetworkComputeBridgeClient : public ServiceClient {
 public:
    NetworkComputeBridgeClient() = default;
    ~NetworkComputeBridgeClient() = default;

    // Asynchronous method to request a response to a (now deprecated) NetworkComputeRequest
    std::shared_future<NetworkComputeResultType> NetworkComputeAsync(
        ::bosdyn::api::NetworkComputeRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to request a response to a NetworkComputeRequest
    NetworkComputeResultType NetworkCompute(::bosdyn::api::NetworkComputeRequest& request,
                                            const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to request a list of available NCB models on the robot
    std::shared_future<ListAvailableModelsResultType> ListAvailableModelsAsync(
        ::bosdyn::api::ListAvailableModelsRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to request a list of available NCB models on the robot
    ListAvailableModelsResultType ListAvailableModels(
        ::bosdyn::api::ListAvailableModelsRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    // Sets the QualityOfService enum for the network compute bridge worker client to be used for
    // network selection optimization.
    QualityOfService GetQualityOfService() const override;

    // Set the communication details of the network compute bridge worker service, including the
    // stub and request/response processors
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;

    // End of ServiceClient overrides.

    // Get the default service name the network compute bridge worker service will be registered in
    // the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the network compute bridge worker service that will be
    // registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function for asynchronous NetworkCompute RPC calls to return a result
    void OnNetworkComputeComplete(MessagePumpCallBase* call,
                                  const ::bosdyn::api::NetworkComputeRequest& request,
                                  ::bosdyn::api::NetworkComputeResponse&& response,
                                  const grpc::Status& status,
                                  std::promise<NetworkComputeResultType> promise);

    // Callback function for asynchronous ListAvailableModels RPC calls to return a result
    void OnListAvailableModelsComplete(MessagePumpCallBase* call,
                                       const ::bosdyn::api::ListAvailableModelsRequest& request,
                                       ::bosdyn::api::ListAvailableModelsResponse&& response,
                                       const grpc::Status& status,
                                       std::promise<ListAvailableModelsResultType> promise);

    std::unique_ptr<::bosdyn::api::NetworkComputeBridge::StubInterface> m_stub;

    // Default service name for the network compute bridge worker service.
    static const char* s_default_service_name;

    // Default service type for the network compute bridge worker service.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
