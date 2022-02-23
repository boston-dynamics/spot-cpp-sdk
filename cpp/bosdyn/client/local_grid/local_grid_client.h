/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/local_grid_service.grpc.pb.h>
#include <bosdyn/api/local_grid_service.pb.h>

#include <future>
#include <memory>
#include <string>
#include <vector>

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/common/status.h"
#include "bosdyn/client/local_grid/local_grid_error_codes.h"

namespace bosdyn {

namespace client {

typedef Result<::bosdyn::api::GetLocalGridTypesResponse> LocalGridTypesResultType;
typedef Result<::bosdyn::api::GetLocalGridsResponse> LocalGridsResultType;

// LocalGridClient is the GRPC client for the LocalGrid service defined in local_grid_service.proto.
class LocalGridClient : public ServiceClient {
 public:
    LocalGridClient() { m_RPC_parameters.logging_control = LogRequestMode::kEnabled; }

    ~LocalGridClient() = default;

    // Asynchronous method to request the list of available local grid types.
    std::shared_future<LocalGridTypesResultType> GetLocalGridTypesAsync(
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to request the list of available local grid types.
    LocalGridTypesResultType GetLocalGridTypes(
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to request a set of local grids by type name.
    std::shared_future<LocalGridsResultType> GetLocalGridsAsync(
        const std::vector<std::string>& local_grid_names,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to request a set of local grids by type name.
    LocalGridsResultType GetLocalGrids(
        const std::vector<std::string>& local_grid_names,
        const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the LocalGrid service will be registered in the directory
    // with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the LocalGrid service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    void OnGetLocalGridTypesComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::GetLocalGridTypesRequest& request,
        ::bosdyn::api::GetLocalGridTypesResponse&& response, const grpc::Status& status,
        std::promise<LocalGridTypesResultType> promise);

    void OnGetLocalGridsComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::GetLocalGridsRequest& request,
        ::bosdyn::api::GetLocalGridsResponse&& response, const grpc::Status& status,
        std::promise<LocalGridsResultType> promise);

    std::unique_ptr<::bosdyn::api::LocalGridService::Stub> m_stub;

    // Default service name for the LocalGrid service.
    static const char* s_default_service_name;

    // Default service type for the LocalGrid service.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
