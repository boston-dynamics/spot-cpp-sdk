/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "world_object_client.h"

#include "bosdyn/client/error_codes/sdk_error_code.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* WorldObjectClient::s_default_service_name = "world-objects";

const char* WorldObjectClient::s_service_type = "bosdyn.api.WorldObjectService";

// Asynchronous RPC method to list all world objects with no filters.
std::shared_future<ListWorldObjectsResultType> WorldObjectClient::ListWorldObjectsAsync(
    const RPCParameters& parameters) {
    // Empty list world objects request to get all known world objects (and not apply any filters
    // based on type or timestamp).
    ::bosdyn::api::ListWorldObjectRequest request;
    return ListWorldObjectsAsync(request, parameters);
}

// Synchronous RPC method to list all world objects with no filters.
ListWorldObjectsResultType WorldObjectClient::ListWorldObjects(
    const RPCParameters& parameters) {
    return this->ListWorldObjectsAsync(parameters).get();
}

// Asynchronous RPC method to list all world objects taking the full request, which can
// be used to specify filters on the object type or timestamp.
std::shared_future<ListWorldObjectsResultType> WorldObjectClient::ListWorldObjectsAsync(
    ::bosdyn::api::ListWorldObjectRequest& request,
    const RPCParameters& parameters) {
    std::promise<ListWorldObjectsResultType> response;
    std::shared_future<ListWorldObjectsResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::ListWorldObjectRequest, ::bosdyn::api::ListWorldObjectResponse,
                          ::bosdyn::api::ListWorldObjectResponse>(
            request,
            std::bind(&::bosdyn::api::WorldObjectService::Stub::AsyncListWorldObjects, m_stub.get(), _1, _2,
                      _3),
            std::bind(&WorldObjectClient::OnListWorldObjectsComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);
    return future;
}

// Synchronous RPC method to list all world objects taking the full request, which can
// be used to specify filters on the object type or timestamp.
ListWorldObjectsResultType WorldObjectClient::ListWorldObjects(
    ::bosdyn::api::ListWorldObjectRequest& request,
    const RPCParameters& parameters) {
    return this->ListWorldObjectsAsync(request, parameters).get();
}

// Response Callback for ListWorldObjects request that will return the full ListWorldObjectResponse.
void WorldObjectClient::OnListWorldObjectsComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::ListWorldObjectRequest& request,
    ::bosdyn::api::ListWorldObjectResponse&& response, const grpc::Status& status,
    std::promise<ListWorldObjectsResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::ListWorldObjectResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

// Asynchronous RPC method to mutate (add, change, delete) a world object.
std::shared_future<MutateWorldObjectsResultType> WorldObjectClient::MutateWorldObjectsAsync(
    ::bosdyn::api::MutateWorldObjectRequest& request,
    const RPCParameters& parameters) {
    std::promise<MutateWorldObjectsResultType> response;
    std::shared_future<MutateWorldObjectsResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::MutateWorldObjectRequest, ::bosdyn::api::MutateWorldObjectResponse,
                          ::bosdyn::api::MutateWorldObjectResponse>(
            request,
            std::bind(&::bosdyn::api::WorldObjectService::Stub::AsyncMutateWorldObjects, m_stub.get(), _1, _2,
                      _3),
            std::bind(&WorldObjectClient::OnMutateWorldObjectsComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);
    return future;
}

// Synchronous RPC method to mutate (add, change, delete) a world object.
MutateWorldObjectsResultType WorldObjectClient::MutateWorldObjects(
    ::bosdyn::api::MutateWorldObjectRequest& request,
    const RPCParameters& parameters) {
    return this->MutateWorldObjectsAsync(request, parameters).get();
}

// Response Callback for MutateWorldObjects request that will return the full
// MutateWorldObjectResponse.
void WorldObjectClient::OnMutateWorldObjectsComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::MutateWorldObjectRequest& request,
    ::bosdyn::api::MutateWorldObjectResponse&& response, const grpc::Status& status,
    std::promise<MutateWorldObjectsResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::MutateWorldObjectResponse>(
        status, response, response.status());

    promise.set_value({ret_status, std::move(response)});
}

// Overrides for virtual functions in the base ServiceClient class.
ServiceClient::QualityOfService WorldObjectClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void WorldObjectClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::WorldObjectService::Stub(channel));
}

}  // namespace client

}  // namespace bosdyn
