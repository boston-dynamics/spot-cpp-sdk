/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/time_sync.pb.h>
#include <bosdyn/api/time_sync_service.grpc.pb.h>
#include <bosdyn/api/time_sync_service.pb.h>

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/common/status.h"
#include "bosdyn/client/time_sync/time_sync_error_codes.h"

namespace bosdyn {

namespace client {

typedef Result<::bosdyn::api::TimeSyncUpdateResponse> TimeSyncUpdateResultType;

class TimeSyncClient : public ServiceClient {
 public:
    TimeSyncClient() = default;

    ~TimeSyncClient() = default;

    // Asynchronous method to make a round trip call towards synchronization.
    std::shared_future<TimeSyncUpdateResultType> TimeSyncUpdateAsync(
        ::bosdyn::api::TimeSyncUpdateRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to time sync update.
    TimeSyncUpdateResultType TimeSyncUpdate(
        ::bosdyn::api::TimeSyncUpdateRequest& request,
        const RPCParameters& parameters = RPCParameters());


    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the Timesync service will be registered in the directory
    // with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the Timesync service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    void OnTimeSyncUpdateComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::TimeSyncUpdateRequest& request,
        ::bosdyn::api::TimeSyncUpdateResponse&& response, const grpc::Status& status,
        std::promise<TimeSyncUpdateResultType> promise);


    // Default service name for the Timesync service.
    static const char* s_default_service_name;

    // Default service type for the Timesync service.
    static const char* s_service_type;

    std::unique_ptr<::bosdyn::api::TimeSyncService::Stub> m_stub;
};

}  // namespace client

}  // namespace bosdyn
