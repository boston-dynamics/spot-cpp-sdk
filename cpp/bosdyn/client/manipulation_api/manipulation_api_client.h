/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/manipulation_api.pb.h>
#include <bosdyn/api/manipulation_api_service.grpc.pb.h>
#include <bosdyn/api/manipulation_api_service.pb.h>

#include <future>

#include "bosdyn/client/lease/lease.h"
#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/common/status.h"

namespace bosdyn {

namespace client {

// The return types for each RPC callback that provides the status of the RPC call in addition
// to the data/response message the RPC was returning from the robot.
typedef Result<::bosdyn::api::ManipulationApiResponse> ManipulationApiResultType;
typedef Result<::bosdyn::api::ManipulationApiFeedbackResponse> ManipulationApiFeedbackResultType;
typedef Result<::bosdyn::api::ApiGraspOverrideResponse> OverrideGraspResultType;

/**
 * The ManipulationApi service handles manipulation commands and provides feedback on command
 * status. This creates a client which communicates to the ManipulationApi service and can:
 *    - Send a manipulation command to the robot.
 *    - Request feedback on a manipulation command.
 *    - Override grasp and/or carry states.
 */
class ManipulationApiClient : public ServiceClient {
 public:
    ManipulationApiClient() = default;
    ~ManipulationApiClient() = default;

    // Asynchronous method to issue a manipulation command.
    std::shared_future<ManipulationApiResultType> ManipulationApiAsync(
        ::bosdyn::api::ManipulationApiRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to issue a manipulation command.
    ManipulationApiResultType ManipulationApi(::bosdyn::api::ManipulationApiRequest& request,
                                              const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to request manipulation feedback, which indicates the state
    // of a manipulation command, none of which are considered errors.
    std::shared_future<ManipulationApiFeedbackResultType> ManipulationApiFeedbackAsync(
        ::bosdyn::api::ManipulationApiFeedbackRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to request manipulation feedback.
    ManipulationApiFeedbackResultType ManipulationApiFeedback(
        ::bosdyn::api::ManipulationApiFeedbackRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to override grasp and/or carry states.
    std::shared_future<OverrideGraspResultType> OverrideGraspAsync(
        ::bosdyn::api::ApiGraspOverrideRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to override grasp and/or carry states.
    OverrideGraspResultType OverrideGrasp(::bosdyn::api::ApiGraspOverrideRequest& request,
                                          const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    // Sets the QualityOfService enum for the manipulation API client to be used for network
    // selection optimization.
    QualityOfService GetQualityOfService() const override;

    // Set the communication details for the manipulation API service, including the stub and
    // processors for requests/responses.
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;

    // Override the update from method to set the lease wallet for the manipulation API client
    // to be the same as the lease wallet of the robot object.
    void UpdateServiceFrom(RequestProcessorChain& request_processor_chain,
                           ResponseProcessorChain& response_processor_chain,
                           const std::shared_ptr<LeaseWallet>& lease_wallet) override;
    // End of ServiceClient overrides.

    // Get the default service name the ManipulationApi service will be registered in the directory
    // with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the lease wallet for the manipulation API service.
    std::shared_ptr<LeaseWallet> GetLeaseWallet() { return m_lease_wallet; }

    // Get the default service type for the manipulation API service that will be registered in the
    // directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous calls to issue a manipulation command.
    void OnManipulationApiComplete(MessagePumpCallBase* call,
                                   const ::bosdyn::api::ManipulationApiRequest& request,
                                   ::bosdyn::api::ManipulationApiResponse&& response,
                                   const grpc::Status& status,
                                   std::promise<ManipulationApiResultType> promise);

    // Callback function registered for the asynchronous calls to request command feedback.
    void OnManipulationApiFeedbackComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::ManipulationApiFeedbackRequest& request,
        ::bosdyn::api::ManipulationApiFeedbackResponse&& response, const grpc::Status& status,
        std::promise<ManipulationApiFeedbackResultType> promise);

    // Callback function registered for the asynchronous calls to override grasp and/or carry
    // states.
    void OnOverrideGraspComplete(MessagePumpCallBase* call,
                                 const ::bosdyn::api::ApiGraspOverrideRequest& request,
                                 ::bosdyn::api::ApiGraspOverrideResponse&& response,
                                 const grpc::Status& status,
                                 std::promise<OverrideGraspResultType> promise);

    std::unique_ptr<::bosdyn::api::ManipulationApiService::Stub> m_stub;

    // Default service name for the manipulation API service.
    static const char* s_default_service_name;

    // Default service type for the manipulation API service.
    static const char* s_service_type;

    // Lease wallet for commands.
    std::shared_ptr<LeaseWallet> m_lease_wallet;
};

}  // namespace client

}  // namespace bosdyn
