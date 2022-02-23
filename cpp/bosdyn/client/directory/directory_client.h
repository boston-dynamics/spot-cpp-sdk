/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/client/directory/directory_error_codes.h"

#include <bosdyn/api/directory_service.grpc.pb.h>
#include <bosdyn/api/directory_service.pb.h>
#include <future>

namespace bosdyn {

namespace client {

typedef Result<::bosdyn::api::ListServiceEntriesResponse> DirectoryListResultType;
typedef Result<::bosdyn::api::GetServiceEntryResponse> DirectoryEntryResultType;

class DirectoryClient : public ServiceClient {
 public:
    DirectoryClient() = default;

    ~DirectoryClient() = default;

    // Asynchronous method to get a ListServiceEntriesResponse.
    std::shared_future<DirectoryListResultType> ListServiceEntriesAsync(
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to get a ListServiceEntriesResponse.
    DirectoryListResultType ListServiceEntries(
        const RPCParameters& parameters = RPCParameters());


    // Asynchronous method to get a GetServiceEntryResponse for the corresponding ServiceClient
    // with the given name.
    std::shared_future<DirectoryEntryResultType> GetServiceEntryAsync(
        const std::string& service_name,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to get a GetServiceEntryResponse for the corresponding ServiceClient
    // with the given name.
    DirectoryEntryResultType GetServiceEntry(
        const std::string& service_name,
        const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the Directory service will be registered in the directory
    // with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service authority the Directory service will be registered in the directory
    // with.
    static std::string GetDefaultServiceAuthority() { return s_default_service_authority; }

    // Get the default service type for the Directory service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous calls to get ListServiceEntriesResponse.
    void OnGetListComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::ListServiceEntriesRequest& request,
        ::bosdyn::api::ListServiceEntriesResponse&& response, const grpc::Status& status,
        std::promise<DirectoryListResultType> promise);


    // Callback function registered for the asynchronous calls to get GetServiceEntryResponse.
    void OnGetEntryComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::GetServiceEntryRequest& request,
        ::bosdyn::api::GetServiceEntryResponse&& response, const grpc::Status& status,
        std::promise<DirectoryEntryResultType> promise);

    // Comms stuff.
    std::unique_ptr<::bosdyn::api::DirectoryService::Stub> m_stub;

    // Default service name for the Directory service.
    static const char* s_default_service_name;

    // Default service authority for the Directory service.
    static const char* s_default_service_authority;

    // Default service type for the Directory service.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
