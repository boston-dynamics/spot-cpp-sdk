/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/keepalive/keepalive_client.h"
#include "bosdyn/client/lease/lease_processors.h"
#include "bosdyn/client/lease/lease_resources.h"
#include "bosdyn/common/success_condition.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* KeepaliveClient::s_default_service_name = "keepalive";

const char* KeepaliveClient::s_service_type = "bosdyn.api.keepalive.KeepaliveService";

std::shared_future<ModifyPolicyResultType> KeepaliveClient::ModifyPolicyAsync(
    ::bosdyn::api::keepalive::ModifyPolicyRequest& request, const RPCParameters& parameters) {
    std::promise<ModifyPolicyResultType> response;
    std::shared_future<ModifyPolicyResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::keepalive::ModifyPolicyRequest,
                          ::bosdyn::api::keepalive::ModifyPolicyResponse,
                          ::bosdyn::api::keepalive::ModifyPolicyResponse>(
            request,
            std::bind(&::bosdyn::api::keepalive::KeepaliveService::Stub::AsyncModifyPolicy,
                      m_stub.get(), _1, _2, _3),
            std::bind(&KeepaliveClient::OnModifyPolicyComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

ModifyPolicyResultType KeepaliveClient::ModifyPolicy(
    ::bosdyn::api::keepalive::ModifyPolicyRequest& request, const RPCParameters& parameters) {
    return ModifyPolicyAsync(request, parameters).get();
}

void KeepaliveClient::OnModifyPolicyComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::keepalive::ModifyPolicyRequest& request,
    ::bosdyn::api::keepalive::ModifyPolicyResponse&& response, const grpc::Status& status,
    std::promise<ModifyPolicyResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseAndGetFinalStatus<::bosdyn::api::keepalive::ModifyPolicyResponse>(
            status, response, response.status());
    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<GetStatusResultType> KeepaliveClient::GetStatusAsync(
    ::bosdyn::api::keepalive::GetStatusRequest& request, const RPCParameters& parameters) {
    std::promise<GetStatusResultType> response;
    std::shared_future<GetStatusResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time = InitiateAsyncCall<::bosdyn::api::keepalive::GetStatusRequest,
                                                      ::bosdyn::api::keepalive::GetStatusResponse,
                                                      ::bosdyn::api::keepalive::GetStatusResponse>(
        request,
        std::bind(&::bosdyn::api::keepalive::KeepaliveService::Stub::AsyncGetStatus, m_stub.get(),
                  _1, _2, _3),
        std::bind(&KeepaliveClient::OnGetStatusComplete, this, _1, _2, _3, _4, _5),
        std::move(response), parameters);

    return future;
}

GetStatusResultType KeepaliveClient::GetStatus(::bosdyn::api::keepalive::GetStatusRequest& request,
                                               const RPCParameters& parameters) {
    return GetStatusAsync(request, parameters).get();
}

void KeepaliveClient::OnGetStatusComplete(MessagePumpCallBase* call,
                                          const ::bosdyn::api::keepalive::GetStatusRequest& request,
                                          ::bosdyn::api::keepalive::GetStatusResponse&& response,
                                          const grpc::Status& status,
                                          std::promise<GetStatusResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseAndGetFinalStatus<::bosdyn::api::keepalive::GetStatusResponse>(
            status, response, SDKErrorCode::Success);
    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<CheckInResultType> KeepaliveClient::CheckInAsync(
    ::bosdyn::api::keepalive::CheckInRequest& request, const RPCParameters& parameters) {
    std::promise<CheckInResultType> response;
    std::shared_future<CheckInResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time = InitiateAsyncCall<::bosdyn::api::keepalive::CheckInRequest,
                                                      ::bosdyn::api::keepalive::CheckInResponse,
                                                      ::bosdyn::api::keepalive::CheckInResponse>(
        request,
        std::bind(&::bosdyn::api::keepalive::KeepaliveService::Stub::AsyncCheckIn, m_stub.get(), _1,
                  _2, _3),
        std::bind(&KeepaliveClient::OnCheckInComplete, this, _1, _2, _3, _4, _5),
        std::move(response), parameters);

    return future;
}

CheckInResultType KeepaliveClient::CheckIn(::bosdyn::api::keepalive::CheckInRequest& request,
                                           const RPCParameters& parameters) {
    return CheckInAsync(request, parameters).get();
}

void KeepaliveClient::OnCheckInComplete(MessagePumpCallBase* call,
                                        const ::bosdyn::api::keepalive::CheckInRequest& request,
                                        ::bosdyn::api::keepalive::CheckInResponse&& response,
                                        const grpc::Status& status,
                                        std::promise<CheckInResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseAndGetFinalStatus<::bosdyn::api::keepalive::CheckInResponse>(
            status, response, SDKErrorCode::Success);
    promise.set_value({ret_status, std::move(response)});
}

// Start of ServiceClient overrides.
ServiceClient::QualityOfService KeepaliveClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void KeepaliveClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::keepalive::KeepaliveService::Stub(channel));
}

void KeepaliveClient::UpdateServiceFrom(RequestProcessorChain& request_processor_chain,
                                        ResponseProcessorChain& response_processor_chain,
                                        const std::shared_ptr<LeaseWallet>& lease_wallet) {
    // Set the request and response processors.
    m_request_processor_chain = request_processor_chain;
    m_response_processor_chain = response_processor_chain;
}
// End of ServiceClient overrides.

}  // namespace client

}  // namespace bosdyn
