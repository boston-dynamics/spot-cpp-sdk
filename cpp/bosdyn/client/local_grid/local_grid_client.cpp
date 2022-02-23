/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/local_grid/local_grid_client.h"

#include <memory>
#include <string>
#include <vector>

#include "bosdyn/common/success_condition.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* LocalGridClient::s_default_service_name = "local-grid-service";

const char* LocalGridClient::s_service_type = "bosdyn.api.LocalGridService";

std::shared_future<LocalGridTypesResultType> LocalGridClient::GetLocalGridTypesAsync(
    const RPCParameters& parameters) {
    std::promise<LocalGridTypesResultType> response;
    std::shared_future<LocalGridTypesResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::GetLocalGridTypesRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::GetLocalGridTypesRequest, ::bosdyn::api::GetLocalGridTypesResponse,
                          ::bosdyn::api::GetLocalGridTypesResponse>(
            request,
            std::bind(&::bosdyn::api::LocalGridService::Stub::AsyncGetLocalGridTypes, m_stub.get(), _1, _2,
                      _3),
            std::bind(&LocalGridClient::OnGetLocalGridTypesComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

LocalGridTypesResultType LocalGridClient::GetLocalGridTypes(
    const RPCParameters& parameters) {
    return GetLocalGridTypesAsync(parameters).get();
}

void LocalGridClient::OnGetLocalGridTypesComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::GetLocalGridTypesRequest& request,
    ::bosdyn::api::GetLocalGridTypesResponse&& response, const grpc::Status& status,
    std::promise<LocalGridTypesResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::GetLocalGridTypesResponse>(
        status, response, SDKErrorCode::Success);
    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<LocalGridsResultType> LocalGridClient::GetLocalGridsAsync(
    const std::vector<std::string>& local_grid_names,
    const RPCParameters& parameters) {
    std::promise<LocalGridsResultType> response;
    std::shared_future<LocalGridsResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::GetLocalGridsRequest request;
    for (const std::string name : local_grid_names) {
        ::bosdyn::api::LocalGridRequest* req = request.add_local_grid_requests();
        req->set_local_grid_type_name(name);
    }

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::GetLocalGridsRequest, ::bosdyn::api::GetLocalGridsResponse,
                          ::bosdyn::api::GetLocalGridsResponse>(
            request,
            std::bind(&::bosdyn::api::LocalGridService::Stub::AsyncGetLocalGrids, m_stub.get(), _1, _2, _3),
            std::bind(&LocalGridClient::OnGetLocalGridsComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

LocalGridsResultType LocalGridClient::GetLocalGrids(
    const std::vector<std::string>& local_grid_names,
    const RPCParameters& parameters) {
    return GetLocalGridsAsync(local_grid_names, parameters).get();
}

void LocalGridClient::OnGetLocalGridsComplete(
    MessagePumpCallBase*, const ::bosdyn::api::GetLocalGridsRequest&,
    ::bosdyn::api::GetLocalGridsResponse&& response, const grpc::Status& status,
    std::promise<LocalGridsResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::GetLocalGridsResponse>(
        status, response, SDKErrorCode::Success);

    if (!ret_status) {
        promise.set_value({ret_status, std::move(response)});
        return;
    }

    // Iterate through the LocalGridResponse.status to verify there are no errors.
    for (const auto& grid_response : response.local_grid_responses()) {
        std::error_code code = grid_response.status();
        if (code != SuccessCondition::Success) {
            std::cerr << "[[[[[ LOCAL GRID RESPONSE ERROR CODE = " << code << " ]]]]]" << std::endl;
            promise.set_value({::bosdyn::common::Status(code, "LocalGridResponse ::bosdyn::common::Status unsuccessful"),
                               std::move(response)});
            return;
        }
    }

    promise.set_value({ret_status, std::move(response)});
}

// Start of ServiceClient overrides.
ServiceClient::QualityOfService LocalGridClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void LocalGridClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::LocalGridService::Stub(channel));
}
// End of ServiceClient overrides.

}  // namespace client

}  // namespace bosdyn
