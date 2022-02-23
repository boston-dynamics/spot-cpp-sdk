/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include "bosdyn/client/service_client/service_client.h"

#include <bosdyn/api/robot_id_service.grpc.pb.h>
#include <bosdyn/api/robot_id_service.pb.h>
#include <future>

namespace bosdyn {

namespace client {

typedef Result<::bosdyn::api::RobotIdResponse> RobotIdResultType;

class RobotIdClient : public ServiceClient {
 public:
    RobotIdClient() = default;

    ~RobotIdClient() = default;

    // Asynchronous method to get a RobotIdResponse.
    std::shared_future<RobotIdResultType> GetAsync(
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to get a RobotIdResponse.
    RobotIdResultType Get(const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the RobotId service will be registered in the directory
    // with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service authority the RobotId service will be registered in the directory
    // with.
    static std::string GetDefaultServiceAuthority() { return s_default_service_authority; }

    // Get the default service type for the RobotId service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous calls.
    void OnGetRobotIdComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::RobotIdRequest& request,
        ::bosdyn::api::RobotIdResponse&& response, const grpc::Status& status,
        std::promise<RobotIdResultType> promise);

    std::unique_ptr<::bosdyn::api::RobotIdService::Stub> m_stub;

    // Default service name for the RobotId service.
    static const char* s_default_service_name;

    // Default service authority for the RobotId service.
    static const char* s_default_service_authority;

    // Default service type for the RobotId service.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
