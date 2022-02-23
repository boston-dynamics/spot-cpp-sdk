/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <google/protobuf/util/time_util.h>

#include <bosdyn/api/license.pb.h>
#include <bosdyn/api/license_service.grpc.pb.h>
#include <bosdyn/api/license_service.pb.h>

#include <future>

#include "bosdyn/client/service_client/service_client.h"

namespace bosdyn {

namespace client {

typedef Result<::bosdyn::api::GetLicenseInfoResponse> GetLicenseInfoResultType;
typedef Result<::bosdyn::api::GetFeatureEnabledResponse> GetFeatureEnabledResultType;

class LicenseClient : public ServiceClient {
 public:
    LicenseClient() = default;

    ~LicenseClient() = default;

    std::shared_future<GetLicenseInfoResultType> GetLicenseInfoAsync(
        const RPCParameters& parameters = RPCParameters());

    GetLicenseInfoResultType GetLicenseInfo(const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetFeatureEnabledResultType> GetFeatureEnabledAsync(
        ::bosdyn::api::GetFeatureEnabledRequest& request, const RPCParameters& parameters = RPCParameters());

    GetFeatureEnabledResultType GetFeatureEnabled(
        ::bosdyn::api::GetFeatureEnabledRequest& request, const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the RobotState service will be registered in the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the RobotState service that will be registered in the
    // directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    void OnGetLicenseInfoComplete(MessagePumpCallBase* call,
                                  const ::bosdyn::api::GetLicenseInfoRequest& request,
                                  ::bosdyn::api::GetLicenseInfoResponse&& response,
                                  const grpc::Status& status,
                                  std::promise<GetLicenseInfoResultType> promise);

    void OnGetFeatureEnabledComplete(MessagePumpCallBase* call,
                                     const ::bosdyn::api::GetFeatureEnabledRequest& request,
                                     ::bosdyn::api::GetFeatureEnabledResponse&& response,
                                     const grpc::Status& status,
                                     std::promise<GetFeatureEnabledResultType> promise);

    std::unique_ptr<::bosdyn::api::LicenseService::Stub> m_stub;

    // Default service name for the LogAnnotation service.
    static const char* s_default_service_name;

    // Default service type for the LogAnnotation service.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
