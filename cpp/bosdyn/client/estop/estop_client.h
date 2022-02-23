/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/estop.pb.h>
#include <bosdyn/api/estop_service.grpc.pb.h>
#include <bosdyn/api/estop_service.pb.h>

#include <future>

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/common/status.h"
#include "bosdyn/client/estop/estop_error_codes.h"

namespace bosdyn {

namespace client {

typedef Result<::bosdyn::api::RegisterEstopEndpointResponse> RegisterEstopEndpointResultType;
typedef Result<::bosdyn::api::DeregisterEstopEndpointResponse>
    DeregisterEstopEndpointResultType;
typedef Result<::bosdyn::api::GetEstopConfigResponse> GetEstopConfigResultType;
typedef Result<::bosdyn::api::SetEstopConfigResponse> SetEstopConfigResultType;
typedef Result<::bosdyn::api::EstopCheckInResponse> EstopCheckInResultType;
typedef Result<::bosdyn::api::GetEstopSystemStatusResponse> GetEstopSystemStatusResultType;

// EstopClient for using the E-Stop service.
class EstopClient : public ServiceClient {
 public:
    EstopClient() = default;
    ~EstopClient() = default;

    // Asynchronous method to register an E-Stop endpoint in a target configuration.
    std::shared_future<RegisterEstopEndpointResultType> RegisterEstopEndpointAsync(
        ::bosdyn::api::RegisterEstopEndpointRequest& register_request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to register an E-Stop endpoint.
    RegisterEstopEndpointResultType RegisterEstopEndpoint(
        ::bosdyn::api::RegisterEstopEndpointRequest& register_request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to deregister the endpoint in the target configuration.
    std::shared_future<DeregisterEstopEndpointResultType> DeregisterEstopEndpointAsync(
        ::bosdyn::api::DeregisterEstopEndpointRequest& deregister_request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to deregister an E-Stop endpoint.
    DeregisterEstopEndpointResultType DeregisterEstopEndpoint(
        ::bosdyn::api::DeregisterEstopEndpointRequest& deregister_request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to return the E-Stop configuration of the robot.
    std::shared_future<GetEstopConfigResultType> GetEstopConfigAsync(
        const std::string& target_config_id = "",
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to return the E-Stop configuration of the robot.
    GetEstopConfigResultType GetEstopConfig(
        const std::string& target_config_id = "",
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to change the E-Stop configuration on robot.
    std::shared_future<SetEstopConfigResultType> SetEstopConfigAsync(
        ::bosdyn::api::EstopConfig& estop_config, const std::string& target_config_id,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to change the E-Stop configuration on robot.
    SetEstopConfigResultType SetEstopConfig(
        ::bosdyn::api::EstopConfig& estop_config, const std::string& target_config_id,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to return the E-Stop status of the robot.
    std::shared_future<GetEstopSystemStatusResultType> GetEstopStatusAsync(
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to return the E-Stop status of the robot.
    GetEstopSystemStatusResultType GetEstopStatus(
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to checkin with the E-Stop service of the robot.
    std::shared_future<EstopCheckInResultType> EstopCheckInAsync(
        ::bosdyn::api::EstopCheckInRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to checkin with the E-Stop service of the robot.
    EstopCheckInResultType EstopCheckIn(
        ::bosdyn::api::EstopCheckInRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the E-Stop service will be registered in the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the E-stop service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous calls to register an endpoint.
    void OnRegisterEstopEndpointComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::RegisterEstopEndpointRequest& request,
        ::bosdyn::api::RegisterEstopEndpointResponse&& response, const grpc::Status& status,
        std::promise<RegisterEstopEndpointResultType> promise);

    // Callback function registered for the asynchronous calls to deregister an endpoint.
    void OnDeregisterEstopEndpointComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::DeregisterEstopEndpointRequest& request,
        ::bosdyn::api::DeregisterEstopEndpointResponse&& response, const grpc::Status& status,
        std::promise<DeregisterEstopEndpointResultType> promise);

    // Callback function registered for the asynchronous calls to get the E-Stop config.
    void OnGetEstopConfigComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::GetEstopConfigRequest& request,
        ::bosdyn::api::GetEstopConfigResponse&& response, const grpc::Status& status,
        std::promise<GetEstopConfigResultType> promise);

    // Callback function registered for the asynchronous calls to set the E-Stop config.
    void OnSetEstopConfigComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::SetEstopConfigRequest& request,
        ::bosdyn::api::SetEstopConfigResponse&& response, const grpc::Status& status,
        std::promise<SetEstopConfigResultType> promise);

    // Callback function registered for the asynchronous calls to get the E-Stop status.
    void OnGetEstopStatusComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::GetEstopSystemStatusRequest& request,
        ::bosdyn::api::GetEstopSystemStatusResponse&& response, const grpc::Status& status,
        std::promise<GetEstopSystemStatusResultType> promise);

    // Callback function registered for the asynchronous calls to checkin with the E-Stop service.
    void OnEstopCheckInComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::EstopCheckInRequest& request,
        ::bosdyn::api::EstopCheckInResponse&& response, const grpc::Status& status,
        std::promise<EstopCheckInResultType> promise);

    std::unique_ptr<::bosdyn::api::EstopService::Stub> m_stub;

    // Default service name for the E-Stop service.
    static const char* s_default_service_name;

    // Default service type for the E-Stop service.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
