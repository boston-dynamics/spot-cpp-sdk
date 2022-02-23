/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/payload/payload_client.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* PayloadClient::s_default_service_name = "payload";

const char* PayloadClient::s_service_type = "bosdyn.api.PayloadService";

std::shared_future<ListPayloadsResultType> PayloadClient::ListPayloadsAsync(
    const RPCParameters& parameters) {
    std::promise<ListPayloadsResultType> response;
    std::shared_future<ListPayloadsResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::ListPayloadsRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::ListPayloadsRequest, ::bosdyn::api::ListPayloadsResponse,
                          ::bosdyn::api::ListPayloadsResponse>(
            request,
            std::bind(&::bosdyn::api::PayloadService::Stub::AsyncListPayloads, m_stub.get(), _1, _2, _3),
            std::bind(&PayloadClient::OnListPayloadsComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

ListPayloadsResultType PayloadClient::ListPayloads(
    const RPCParameters& parameters) {
    return ListPayloadsAsync().get();
}

void PayloadClient::OnListPayloadsComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::ListPayloadsRequest& request,
    ::bosdyn::api::ListPayloadsResponse&& response, const grpc::Status& status,
    std::promise<ListPayloadsResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::ListPayloadsResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

ServiceClient::QualityOfService PayloadClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void PayloadClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::PayloadService::Stub(channel));
}
// End of ServiceClient overrides.

}  // namespace client

}  // namespace bosdyn
