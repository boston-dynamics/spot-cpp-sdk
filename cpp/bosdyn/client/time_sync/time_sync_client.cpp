/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/time_sync/time_sync_client.h"

#include "bosdyn/client/error_codes/rpc_error_code.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* TimeSyncClient::s_default_service_name = "time-sync";
const char* TimeSyncClient::s_service_type = "bosdyn.api.TimeSyncService";

std::shared_future<TimeSyncUpdateResultType> TimeSyncClient::TimeSyncUpdateAsync(
    ::bosdyn::api::TimeSyncUpdateRequest& request,
    const RPCParameters& parameters) {
    std::promise<TimeSyncUpdateResultType> response;
    std::shared_future<TimeSyncUpdateResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::TimeSyncUpdateRequest, ::bosdyn::api::TimeSyncUpdateResponse,
                          ::bosdyn::api::TimeSyncUpdateResponse>(
            request,
            std::bind(&::bosdyn::api::TimeSyncService::Stub::AsyncTimeSyncUpdate, m_stub.get(), _1, _2, _3),
            std::bind(&TimeSyncClient::OnTimeSyncUpdateComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

TimeSyncUpdateResultType TimeSyncClient::TimeSyncUpdate(
    ::bosdyn::api::TimeSyncUpdateRequest& request,
    const RPCParameters& parameters) {
    return TimeSyncUpdateAsync(request, parameters).get();
}

void TimeSyncClient::OnTimeSyncUpdateComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::TimeSyncUpdateRequest& request,
    ::bosdyn::api::TimeSyncUpdateResponse&& response, const grpc::Status& status,
    std::promise<TimeSyncUpdateResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::TimeSyncUpdateResponse>(
        status, response, SDKErrorCode::Success);
    promise.set_value({ret_status, std::move(response)});
}


ServiceClient::QualityOfService TimeSyncClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void TimeSyncClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::TimeSyncService::Stub(channel));
}

}  // namespace client

}  // namespace bosdyn
