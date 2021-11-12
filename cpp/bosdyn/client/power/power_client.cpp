/**
 * Copyright (c) 2021 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "power_client.h"

#include "bosdyn/client/error_codes/sdk_error_code.h"
#include "bosdyn/client/lease/lease_processors.h"
#include "bosdyn/client/lease/lease_resources.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* PowerClient::s_default_service_name = "power";

const char* PowerClient::s_service_type = "bosdyn.api.PowerService";

std::shared_future<PowerCommandResultType> PowerClient::PowerCommandAsync(
    ::bosdyn::api::PowerCommandRequest& request, const RPCParameters& parameters) {
    std::promise<PowerCommandResultType> response;
    std::shared_future<PowerCommandResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");
    // Run a lease processor function to attempt to automatically apply a lease to the request if
    // a lease is not already set.
    auto lease_status = ProcessRequestWithLease(&request, m_lease_wallet.get(),
                                                ::bosdyn::client::kBodyResource);
    if (!lease_status) {
        // Failed to set a lease with the lease wallet. Return early since the request will fail without a lease.
        response.set_value({lease_status, {}});
        return future;
    }

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::PowerCommandRequest, ::bosdyn::api::PowerCommandResponse,
                          ::bosdyn::api::PowerCommandResponse>(
            request,
            std::bind(&::bosdyn::api::PowerService::Stub::AsyncPowerCommand, m_stub.get(), _1, _2, _3),
            std::bind(&PowerClient::OnPowerCommandComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

PowerCommandResultType PowerClient::PowerCommand(
    ::bosdyn::api::PowerCommandRequest& request, const RPCParameters& parameters) {
    return PowerCommandAsync(request, parameters).get();
}

void PowerClient::OnPowerCommandComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::PowerCommandRequest& request,
    ::bosdyn::api::PowerCommandResponse&& response, const grpc::Status& status,
    std::promise<PowerCommandResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseWithLeaseAndGetFinalStatus<::bosdyn::api::PowerCommandResponse>(
        status, response, response.status(), m_lease_wallet.get());

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<PowerCommandFeedbackResultType> PowerClient::PowerCommandFeedbackAsync(
    unsigned int id, const RPCParameters& parameters) {
    ::bosdyn::api::PowerCommandFeedbackRequest request;
    request.set_power_command_id(id);
    return PowerCommandFeedbackAsync(request, parameters);
}

std::shared_future<PowerCommandFeedbackResultType> PowerClient::PowerCommandFeedbackAsync(
    ::bosdyn::api::PowerCommandFeedbackRequest& request,
    const RPCParameters& parameters) {
    std::promise<PowerCommandFeedbackResultType> response;
    std::shared_future<PowerCommandFeedbackResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::PowerCommandFeedbackRequest, ::bosdyn::api::PowerCommandFeedbackResponse,
                          ::bosdyn::api::PowerCommandFeedbackResponse>(
            request,
            std::bind(&::bosdyn::api::PowerService::Stub::AsyncPowerCommandFeedback, m_stub.get(), _1, _2,
                      _3),
            std::bind(&PowerClient::OnPowerCommandFeedbackComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

PowerCommandFeedbackResultType PowerClient::PowerCommandFeedback(
    ::bosdyn::api::PowerCommandFeedbackRequest& request,
    const RPCParameters& parameters) {
    return PowerCommandFeedbackAsync(request, parameters).get();
}

PowerCommandFeedbackResultType PowerClient::PowerCommandFeedback(
    unsigned int id, const RPCParameters& parameters) {
    return PowerCommandFeedbackAsync(id, parameters).get();
}

void PowerClient::OnPowerCommandFeedbackComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::PowerCommandFeedbackRequest& request,
    ::bosdyn::api::PowerCommandFeedbackResponse&& response, const grpc::Status& status,
    std::promise<PowerCommandFeedbackResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::PowerCommandFeedbackResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

ServiceClient::QualityOfService PowerClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void PowerClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::PowerService::Stub(channel));
}

void PowerClient::UpdateServiceFrom(
    RequestProcessorChain& request_processor_chain,
    ResponseProcessorChain& response_processor_chain,
    const std::shared_ptr<LeaseWallet>& lease_wallet) {
    // Set the lease wallet.
    m_lease_wallet = lease_wallet;

    // Set the request and response processors.
    m_request_processor_chain = request_processor_chain;
    m_response_processor_chain = response_processor_chain;
}

}  // namespace client

}  // namespace bosdyn
