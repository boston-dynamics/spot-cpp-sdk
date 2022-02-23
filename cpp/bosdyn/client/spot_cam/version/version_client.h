/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/client/spot_cam/version/version_error_codes.h"

#include <bosdyn/api/spot_cam/service.grpc.pb.h>
#include <bosdyn/api/spot_cam/service.pb.h>
#include <future>

namespace bosdyn {

namespace client {

namespace spot_cam {

typedef Result<::bosdyn::api::spot_cam::GetSoftwareVersionResponse> GetSoftwareVersionResultType;

class VersionClient : public ServiceClient {
 public:
    VersionClient() = default;

    ~VersionClient() = default;

    std::shared_future<GetSoftwareVersionResultType> GetSoftwareVersionAsync(
        const RPCParameters& parameters = RPCParameters());

    GetSoftwareVersionResultType GetSoftwareVersion(
        const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the spotcam version service will be registered in the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the spot cam version service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous grpc calls.
    void OnGetSoftwareVersionComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::GetSoftwareVersionRequest& request,
        ::bosdyn::api::spot_cam::GetSoftwareVersionResponse&& response, const grpc::Status& status,
        std::promise<GetSoftwareVersionResultType> promise);

    std::unique_ptr<::bosdyn::api::spot_cam::VersionService::Stub> m_stub;

    // Default service name for the spotcam version service.
    static const char* s_default_service_name;

    // Default service type for the spotcam version service.
    static const char* s_service_type;
};

}  // namespace spot_cam
}  // namespace client

}  // namespace bosdyn
