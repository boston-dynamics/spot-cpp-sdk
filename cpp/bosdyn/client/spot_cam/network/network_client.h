/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <map>

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/client/spot_cam/network/network_error_codes.h"

#include <bosdyn/api/spot_cam/service.grpc.pb.h>
#include <bosdyn/api/spot_cam/service.pb.h>
#include <future>

namespace bosdyn {

namespace client {

namespace spot_cam {

typedef Result<::bosdyn::api::spot_cam::SetICEConfigurationResponse> SetICEConfigurationResultType;
typedef Result<::bosdyn::api::spot_cam::GetICEConfigurationResponse> GetICEConfigurationResultType;

class NetworkClient : public ServiceClient {
 public:
    NetworkClient() = default;

    ~NetworkClient() = default;

    std::shared_future<SetICEConfigurationResultType> SetICEConfigurationAsync(
        ::bosdyn::api::spot_cam::SetICEConfigurationRequest& request,
        const RPCParameters& parameters = RPCParameters());
    SetICEConfigurationResultType SetICEConfiguration(
        ::bosdyn::api::spot_cam::SetICEConfigurationRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetICEConfigurationResultType> GetICEConfigurationAsync(
        const RPCParameters& parameters = RPCParameters());

    GetICEConfigurationResultType GetICEConfiguration(
        const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the spot cam network service will be registered in the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the spot cam network service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous grpc calls.
    void OnSetICEConfigurationComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::SetICEConfigurationRequest& request,
        ::bosdyn::api::spot_cam::SetICEConfigurationResponse&& response, const grpc::Status& status,
        std::promise<SetICEConfigurationResultType> promise);
    void OnGetICEConfigurationComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::GetICEConfigurationRequest& request,
        ::bosdyn::api::spot_cam::GetICEConfigurationResponse&& response, const grpc::Status& status,
        std::promise<GetICEConfigurationResultType> promise);

    std::unique_ptr<::bosdyn::api::spot_cam::NetworkService::Stub> m_stub;

    // Default service name for the spot cam network service.
    static const char* s_default_service_name;

    // Default service type for the spotcam network service.
    static const char* s_service_type;
};

}  // namespace spot_cam
}  // namespace client

}  // namespace bosdyn
