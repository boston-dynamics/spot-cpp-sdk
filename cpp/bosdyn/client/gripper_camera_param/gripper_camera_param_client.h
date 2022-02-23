/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <future>

#include <bosdyn/api/gripper_camera_param_service.grpc.pb.h>
#include <bosdyn/api/gripper_camera_param_service.pb.h>

#include "bosdyn/client/service_client/service_client.h"

namespace bosdyn {

namespace client {

typedef Result<::bosdyn::api::GripperCameraParamResponse> GripperCameraSetParamResponseType;
typedef Result<::bosdyn::api::GripperCameraGetParamResponse> GripperCameraGetParamResponseType;

// GripperCameraParamClient is the GRPC client for the gripper camera parameter service defined in
// gripper_camera_param_service.proto.
class GripperCameraParamClient : public ServiceClient {
 public:
    GripperCameraParamClient() = default;

    ~GripperCameraParamClient() = default;

    // Synchronous method to request to set gripper camera parameters.
    GripperCameraSetParamResponseType SetGripperCameraParams(
        ::bosdyn::api::GripperCameraParamRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to request to set gripper camera parameters.
    std::shared_future<GripperCameraSetParamResponseType> SetGripperCameraParamsAsync(
        ::bosdyn::api::GripperCameraParamRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to request current gripper camera parameters.
    GripperCameraGetParamResponseType GetGripperCameraParams(
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to request current gripper camera parameters.
    std::shared_future<GripperCameraGetParamResponseType> GetGripperCameraParamsAsync(
        const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;

    // Override the update from method to set the lease wallet for the robot command client
    // to be the same as the lease wallet of the robot object.
    void UpdateServiceFrom(RequestProcessorChain& request_processor_chain,
                           ResponseProcessorChain& response_processor_chain,
                           const std::shared_ptr<LeaseWallet>& lease_wallet) override;
    // End of ServiceClient overrides.

    // Get the default service name the Docking service will be registered in the directory
    // with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the Docking service that will be registered in the
    // directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback functions registered for the asynchronous grpc calls.

    // Callback that will return the GripperCameraParamResponse message after GripperCameraParam rpc
    // returns to the client.
    void OnSetGripperCameraParamsComplete(MessagePumpCallBase* call,
                                          const ::bosdyn::api::GripperCameraParamRequest& request,
                                          ::bosdyn::api::GripperCameraParamResponse&& response,
                                          const grpc::Status& status,
                                          std::promise<GripperCameraSetParamResponseType> promise);

    // Callback that will return the GripperCameraGetParamResponse message after GripperCameraParam
    // rpc returns to the client.
    void OnGetGripperCameraParamsComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::GripperCameraGetParamRequest& request,
        ::bosdyn::api::GripperCameraGetParamResponse&& response, const grpc::Status& status,
        std::promise<GripperCameraGetParamResponseType> promise);

    std::unique_ptr<::bosdyn::api::GripperCameraParamService::Stub> m_stub;

    // Lease wallet for commands.
    std::shared_ptr<LeaseWallet> m_lease_wallet;

    // Default service name for the Docking service.
    static const char* s_default_service_name;

    // Default service type for the Docking service.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
