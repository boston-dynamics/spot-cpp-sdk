/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/directory/directory_client.h"
#include "bosdyn/common/status.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* DirectoryClient::s_default_service_name = "directory";

const char* DirectoryClient::s_default_service_authority = "api.spot.robot";

const char* DirectoryClient::s_service_type = "bosdyn.api.DirectoryService";

std::shared_future<DirectoryListResultType> DirectoryClient::ListServiceEntriesAsync(
    const RPCParameters& parameters) {
    std::promise<DirectoryListResultType> response;
    std::shared_future<DirectoryListResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::ListServiceEntriesRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::ListServiceEntriesRequest, ::bosdyn::api::ListServiceEntriesResponse,
                          ::bosdyn::api::ListServiceEntriesResponse>(
            request,
            std::bind(&::bosdyn::api::DirectoryService::Stub::AsyncListServiceEntries, m_stub.get(), _1, _2,
                      _3),
            std::bind(&DirectoryClient::OnGetListComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

// Set the outstanding response, which will update any futures. Reset it.
void DirectoryClient::OnGetListComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::ListServiceEntriesRequest& request,
    ::bosdyn::api::ListServiceEntriesResponse&& response, const grpc::Status& status,
    std::promise<DirectoryListResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::ListServiceEntriesResponse>(
        status, response, SDKErrorCode::Success);
    if (!ret_status) {
        promise.set_value({ret_status, std::move(response)});
        return;
    }

    promise.set_value({ret_status, std::move(response)});
}

DirectoryListResultType DirectoryClient::ListServiceEntries(
    const RPCParameters& parameters) {
    return ListServiceEntriesAsync(parameters).get();
}


std::shared_future<DirectoryEntryResultType> DirectoryClient::GetServiceEntryAsync(
    const std::string& service_name, const RPCParameters& parameters) {
    std::promise<DirectoryEntryResultType> response;
    std::shared_future<DirectoryEntryResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::GetServiceEntryRequest request;
    request.set_service_name(service_name);

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::GetServiceEntryRequest, ::bosdyn::api::GetServiceEntryResponse,
                          ::bosdyn::api::GetServiceEntryResponse>(
            request,
            std::bind(&::bosdyn::api::DirectoryService::Stub::AsyncGetServiceEntry, m_stub.get(), _1, _2, _3),
            std::bind(&DirectoryClient::OnGetEntryComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

// Set the outstanding response, which will update any futures. Reset it.
void DirectoryClient::OnGetEntryComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::GetServiceEntryRequest& request,
    ::bosdyn::api::GetServiceEntryResponse&& response, const grpc::Status& status,
    std::promise<DirectoryEntryResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::GetServiceEntryResponse>(
        status, response, response.status());
    if (!ret_status) {
        promise.set_value({ret_status, std::move(response)});
        return;
    }

    promise.set_value({ret_status, std::move(response)});
}

DirectoryEntryResultType DirectoryClient::GetServiceEntry(
    const std::string& service_name, const RPCParameters& parameters) {
    return GetServiceEntryAsync(service_name, parameters).get();
}

ServiceClient::QualityOfService DirectoryClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void DirectoryClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::DirectoryService::Stub(channel));
}

}  // namespace client

}  // namespace bosdyn
