/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
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

// Power Commands

std::shared_future<PowerCommandResultType> PowerClient::PowerCommandAsync(
    ::bosdyn::api::PowerCommandRequest& request, const RPCParameters& parameters) {
    std::promise<PowerCommandResultType> response;
    std::shared_future<PowerCommandResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");
    // Run a lease processor function to attempt to automatically apply a lease to the request if
    // a lease is not already set.
    auto lease_status =
        ProcessRequestWithLease(&request, m_lease_wallet.get(), ::bosdyn::client::kBodyResource);
    if (!lease_status) {
        // Failed to set a lease with the lease wallet. Return early since the request will fail
        // without a lease.
        response.set_value({lease_status, {}});
        return future;
    }

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::PowerCommandRequest, ::bosdyn::api::PowerCommandResponse,
                          ::bosdyn::api::PowerCommandResponse>(
            request,
            std::bind(&::bosdyn::api::PowerService::Stub::AsyncPowerCommand, m_stub.get(), _1, _2,
                      _3),
            std::bind(&PowerClient::OnPowerCommandComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

PowerCommandResultType PowerClient::PowerCommand(::bosdyn::api::PowerCommandRequest& request,
                                                 const RPCParameters& parameters) {
    return PowerCommandAsync(request, parameters).get();
}

void PowerClient::OnPowerCommandComplete(MessagePumpCallBase* call,
                                         const ::bosdyn::api::PowerCommandRequest& request,
                                         ::bosdyn::api::PowerCommandResponse&& response,
                                         const grpc::Status& status,
                                         std::promise<PowerCommandResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseWithLeaseAndGetFinalStatus<::bosdyn::api::PowerCommandResponse>(
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
    ::bosdyn::api::PowerCommandFeedbackRequest& request, const RPCParameters& parameters) {
    std::promise<PowerCommandFeedbackResultType> response;
    std::shared_future<PowerCommandFeedbackResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time = InitiateAsyncCall<::bosdyn::api::PowerCommandFeedbackRequest,
                                                      ::bosdyn::api::PowerCommandFeedbackResponse,
                                                      ::bosdyn::api::PowerCommandFeedbackResponse>(
        request,
        std::bind(&::bosdyn::api::PowerService::Stub::AsyncPowerCommandFeedback, m_stub.get(), _1,
                  _2, _3),
        std::bind(&PowerClient::OnPowerCommandFeedbackComplete, this, _1, _2, _3, _4, _5),
        std::move(response), parameters);

    return future;
}

PowerCommandFeedbackResultType PowerClient::PowerCommandFeedback(
    ::bosdyn::api::PowerCommandFeedbackRequest& request, const RPCParameters& parameters) {
    return PowerCommandFeedbackAsync(request, parameters).get();
}

PowerCommandFeedbackResultType PowerClient::PowerCommandFeedback(unsigned int id,
                                                                 const RPCParameters& parameters) {
    return PowerCommandFeedbackAsync(id, parameters).get();
}

void PowerClient::OnPowerCommandFeedbackComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::PowerCommandFeedbackRequest& request,
    ::bosdyn::api::PowerCommandFeedbackResponse&& response, const grpc::Status& status,
    std::promise<PowerCommandFeedbackResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseAndGetFinalStatus<::bosdyn::api::PowerCommandFeedbackResponse>(
            status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

// Fan Power Commands

std::shared_future<FanPowerCommandResultType> PowerClient::FanPowerCommandAsync(
    ::bosdyn::api::FanPowerCommandRequest& request, const RPCParameters& parameters) {
    std::promise<FanPowerCommandResultType> response;
    std::shared_future<FanPowerCommandResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");
    // Run a lease processor function to attempt to automatically apply a lease to the request if
    // a lease is not already set. Normally client will attempt to grab body lease, so use
    // bodyResource instead of fanResource here and let split happen in service
    auto lease_status =
        ProcessRequestWithLease(&request, m_lease_wallet.get(), ::bosdyn::client::kBodyResource);
    if (!lease_status) {
        // Failed to set a lease with the lease wallet. Return early since the request will fail
        // without a lease.
        response.set_value({lease_status, {}});
        return future;
    }

    MessagePumpCallBase* one_time = InitiateAsyncCall<::bosdyn::api::FanPowerCommandRequest,
                                                      ::bosdyn::api::FanPowerCommandResponse,
                                                      ::bosdyn::api::FanPowerCommandResponse>(
        request,
        std::bind(&::bosdyn::api::PowerService::Stub::AsyncFanPowerCommand, m_stub.get(), _1, _2,
                  _3),
        std::bind(&PowerClient::OnFanPowerCommandComplete, this, _1, _2, _3, _4, _5),
        std::move(response), parameters);

    return future;
}

FanPowerCommandResultType PowerClient::FanPowerCommand(
    ::bosdyn::api::FanPowerCommandRequest& request, const RPCParameters& parameters) {
    return FanPowerCommandAsync(request, parameters).get();
}

void PowerClient::OnFanPowerCommandComplete(MessagePumpCallBase* call,
                                            const ::bosdyn::api::FanPowerCommandRequest& request,
                                            ::bosdyn::api::FanPowerCommandResponse&& response,
                                            const grpc::Status& status,
                                            std::promise<FanPowerCommandResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseWithLeaseAndGetFinalStatus<::bosdyn::api::FanPowerCommandResponse>(
            status, response, response.status(), m_lease_wallet.get());

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<FanPowerCommandFeedbackResultType> PowerClient::FanPowerCommandFeedbackAsync(
    unsigned int id, const RPCParameters& parameters) {
    ::bosdyn::api::FanPowerCommandFeedbackRequest request;
    request.set_command_id(id);
    return FanPowerCommandFeedbackAsync(request, parameters);
}

std::shared_future<FanPowerCommandFeedbackResultType> PowerClient::FanPowerCommandFeedbackAsync(
    ::bosdyn::api::FanPowerCommandFeedbackRequest& request, const RPCParameters& parameters) {
    std::promise<FanPowerCommandFeedbackResultType> response;
    std::shared_future<FanPowerCommandFeedbackResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::FanPowerCommandFeedbackRequest,
                          ::bosdyn::api::FanPowerCommandFeedbackResponse,
                          ::bosdyn::api::FanPowerCommandFeedbackResponse>(
            request,
            std::bind(&::bosdyn::api::PowerService::Stub::AsyncFanPowerCommandFeedback,
                      m_stub.get(), _1, _2, _3),
            std::bind(&PowerClient::OnFanPowerCommandFeedbackComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

FanPowerCommandFeedbackResultType PowerClient::FanPowerCommandFeedback(
    ::bosdyn::api::FanPowerCommandFeedbackRequest& request, const RPCParameters& parameters) {
    return FanPowerCommandFeedbackAsync(request, parameters).get();
}

FanPowerCommandFeedbackResultType PowerClient::FanPowerCommandFeedback(
    unsigned int id, const RPCParameters& parameters) {
    return FanPowerCommandFeedbackAsync(id, parameters).get();
}

void PowerClient::OnFanPowerCommandFeedbackComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::FanPowerCommandFeedbackRequest& request,
    ::bosdyn::api::FanPowerCommandFeedbackResponse&& response, const grpc::Status& status,
    std::promise<FanPowerCommandFeedbackResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseAndGetFinalStatus<::bosdyn::api::FanPowerCommandFeedbackResponse>(
            status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

// Power Client Commands

ServiceClient::QualityOfService PowerClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void PowerClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::PowerService::Stub(channel));
}

void PowerClient::UpdateServiceFrom(RequestProcessorChain& request_processor_chain,
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
