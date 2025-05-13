/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "network_compute_bridge_client.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* NetworkComputeBridgeClient::s_default_service_name = "network-compute-bridge";

const char* NetworkComputeBridgeClient::s_service_type = "bosdyn.api.NetworkComputeBridge";

std::shared_future<NetworkComputeResultType> NetworkComputeBridgeClient::NetworkComputeAsync(
    ::bosdyn::api::NetworkComputeRequest& request, const RPCParameters& parameters) {
    std::promise<NetworkComputeResultType> response;
    std::shared_future<NetworkComputeResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time = InitiateAsyncCall<::bosdyn::api::NetworkComputeRequest,
                                                      ::bosdyn::api::NetworkComputeResponse,
                                                      ::bosdyn::api::NetworkComputeResponse>(
        request,
        std::bind(&::bosdyn::api::NetworkComputeBridge::StubInterface::AsyncNetworkCompute,
                  m_stub.get(), _1, _2, _3),
        std::bind(&NetworkComputeBridgeClient::OnNetworkComputeComplete, this, _1, _2, _3, _4, _5),
        std::move(response), parameters);
    return future;
}

void NetworkComputeBridgeClient::OnNetworkComputeComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::NetworkComputeRequest& request,
    ::bosdyn::api::NetworkComputeResponse&& response, const grpc::Status& status,
    std::promise<NetworkComputeResultType> promise) {
    std::cout << "On network compute complete\n";
    ::bosdyn::common::Status ret_status =
        ProcessResponseAndGetFinalStatus<::bosdyn::api::NetworkComputeResponse>(status, response,
                                                                                response.status());
    std::cout << "Ret status: " << ret_status.DebugString() << std::endl;
    promise.set_value({ret_status, std::move(response)});
}

NetworkComputeResultType NetworkComputeBridgeClient::NetworkCompute(
    ::bosdyn::api::NetworkComputeRequest& request, const RPCParameters& parameters) {
    return NetworkComputeAsync(request, parameters).get();
}

std::shared_future<ListAvailableModelsResultType>
NetworkComputeBridgeClient::ListAvailableModelsAsync(
    ::bosdyn::api::ListAvailableModelsRequest& request, const RPCParameters& parameters) {
    std::promise<ListAvailableModelsResultType> response;
    std::shared_future<ListAvailableModelsResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time = InitiateAsyncCall<::bosdyn::api::ListAvailableModelsRequest,
                                                      ::bosdyn::api::ListAvailableModelsResponse,
                                                      ::bosdyn::api::ListAvailableModelsResponse>(
        request,
        std::bind(&::bosdyn::api::NetworkComputeBridge::StubInterface::AsyncListAvailableModels,
                  m_stub.get(), _1, _2, _3),
        std::bind(&NetworkComputeBridgeClient::OnListAvailableModelsComplete, this, _1, _2, _3, _4,
                  _5),
        std::move(response), parameters);
    return future;
};

void NetworkComputeBridgeClient::OnListAvailableModelsComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::ListAvailableModelsRequest& request,
    ::bosdyn::api::ListAvailableModelsResponse&& response, const grpc::Status& status,
    std::promise<ListAvailableModelsResultType> promise) {
    std::cout << "On list available models complete\n";
    ::bosdyn::common::Status ret_status =
        ProcessResponseAndGetFinalStatus<::bosdyn::api::ListAvailableModelsResponse>(
            status, response, response.status());
    std::cout << "Ret status: " << ret_status.DebugString() << std::endl;
    promise.set_value({ret_status, std::move(response)});
}

ListAvailableModelsResultType NetworkComputeBridgeClient::ListAvailableModels(
    ::bosdyn::api::ListAvailableModelsRequest& request, const RPCParameters& parameters) {
    return ListAvailableModelsAsync(request, parameters).get();
}

ServiceClient::QualityOfService NetworkComputeBridgeClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void NetworkComputeBridgeClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::NetworkComputeBridge::Stub(channel));
}

}  // namespace client

}  // namespace bosdyn
