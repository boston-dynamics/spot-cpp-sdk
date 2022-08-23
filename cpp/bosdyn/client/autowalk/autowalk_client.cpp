/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/autowalk/autowalk_client.hpp"
#include "bosdyn/client/data_chunk/data_chunking.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

namespace autowalk {

const char* AutowalkClient::s_default_service_name = "autowalk-service";
const char* AutowalkClient::s_service_type = "bosdyn.api.autowalk.AutowalkService";

CompileAutowalkResultType AutowalkClient::CompileAutowalk(
    ::bosdyn::api::autowalk::CompileAutowalkRequest& request, const RPCParameters& parameters) {
    return CompileAutowalkAsync(request, parameters).get();
}

std::shared_future<CompileAutowalkResultType> AutowalkClient::CompileAutowalkAsync(
    ::bosdyn::api::autowalk::CompileAutowalkRequest& request, const RPCParameters& parameters) {
    std::promise<CompileAutowalkResultType> response;
    std::shared_future<CompileAutowalkResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time = InitiateRequestResponseStreamAsyncCallWithChunking<
        ::bosdyn::api::autowalk::CompileAutowalkRequest,
        ::bosdyn::api::autowalk::CompileAutowalkResponse>(
        std::move(request),
        std::bind(&::bosdyn::api::autowalk::AutowalkService::Stub::AsyncCompileAutowalk,
                  m_stub.get(), _1, _2, _3),
        std::bind(&AutowalkClient::OnCompileAutowalkComplete, this, _1, _2, _3, _4, _5),
        std::move(response), parameters);

    return future;
}

void AutowalkClient::OnCompileAutowalkComplete(
    MessagePumpCallBase* call, const std::vector<::bosdyn::api::DataChunk>&& requests,
    std::vector<::bosdyn::api::DataChunk>&& responses, const grpc::Status& status,
    std::promise<CompileAutowalkResultType> promise) {
    std::vector<const ::bosdyn::api::DataChunk*> chunks;
    for (auto& chunk : responses) {
        chunks.push_back(&chunk);
    }

    auto response_result =
        MessageFromDataChunks<::bosdyn::api::autowalk::CompileAutowalkResponse>(chunks);
    if (!response_result) {
        promise.set_value(
            {::bosdyn::common::Status(SDKErrorCode::GenericSDKError,
                                      "Unable to reconstruct response from data chunks"),
             {}});
        return;
    }

    ::bosdyn::api::autowalk::CompileAutowalkResponse&& response = response_result.move();
    auto ret_status =
        ProcessResponseAndGetFinalStatus<::bosdyn::api::autowalk::CompileAutowalkResponse>(
            status, response, response.status());
    promise.set_value({ret_status, std::move(response)});
}

LoadAutowalkResultType AutowalkClient::LoadAutowalk(
    ::bosdyn::api::autowalk::LoadAutowalkRequest& request, const RPCParameters& parameters,
    const std::vector<std::string>& desired_lease_resources) {
    return LoadAutowalkAsync(request, parameters).get();
}

std::shared_future<LoadAutowalkResultType> AutowalkClient::LoadAutowalkAsync(
    ::bosdyn::api::autowalk::LoadAutowalkRequest& request, const RPCParameters& parameters,
    const std::vector<std::string>& desired_lease_resources) {
    std::promise<LoadAutowalkResultType> response;
    std::shared_future<LoadAutowalkResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    // Run a lease processor function to attempt to automatically apply leases to the request.
    auto lease_status = ::bosdyn::client::ProcessRequestWithMultipleLeases(
        &request, m_lease_wallet.get(), desired_lease_resources);
    if (!lease_status) {
        // Failed to set a lease with the lease wallet. Return early since the request will fail
        // without a lease.
        response.set_value({lease_status, {}});
        return future;
    }

    MessagePumpCallBase* one_time = InitiateRequestResponseStreamAsyncCallWithChunking<
        ::bosdyn::api::autowalk::LoadAutowalkRequest,
        ::bosdyn::api::autowalk::LoadAutowalkResponse>(
        std::move(request),
        std::bind(&::bosdyn::api::autowalk::AutowalkService::Stub::AsyncLoadAutowalk, m_stub.get(),
                  _1, _2, _3),
        std::bind(&AutowalkClient::OnLoadAutowalkComplete, this, _1, _2, _3, _4, _5),
        std::move(response), parameters);

    return future;
}

void AutowalkClient::OnLoadAutowalkComplete(MessagePumpCallBase* call,
                                            const std::vector<::bosdyn::api::DataChunk>&& requests,
                                            std::vector<::bosdyn::api::DataChunk>&& responses,
                                            const grpc::Status& status,
                                            std::promise<LoadAutowalkResultType> promise) {
    std::vector<const ::bosdyn::api::DataChunk*> chunks;
    for (auto& chunk : responses) {
        chunks.push_back(&chunk);
    }

    auto response_result =
        MessageFromDataChunks<::bosdyn::api::autowalk::LoadAutowalkResponse>(chunks);
    if (!response_result) {
        promise.set_value(
            {::bosdyn::common::Status(SDKErrorCode::GenericSDKError,
                                      "Unable to reconstruct response from data chunks"),
             {}});
        return;
    }

    auto response = response_result.move();
    auto ret_status =
        ProcessResponseWithMultiLeaseAndGetFinalStatus<::bosdyn::api::autowalk::LoadAutowalkResponse>(
            status, response, response.status(),m_lease_wallet.get());
    promise.set_value({ret_status, std::move(response)});
}

ServiceClient::QualityOfService AutowalkClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void AutowalkClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::autowalk::AutowalkService::Stub(channel));
}

void AutowalkClient::UpdateServiceFrom(RequestProcessorChain& request_processor_chain,
                                       ResponseProcessorChain& response_processor_chain,
                                       const std::shared_ptr<LeaseWallet>& lease_wallet) {
    // Set the lease wallet.
    m_lease_wallet = lease_wallet;

    // Set the request and response processors.
    m_request_processor_chain = request_processor_chain;
    m_response_processor_chain = response_processor_chain;
}

}  // namespace autowalk
}  // namespace client
}  // namespace bosdyn