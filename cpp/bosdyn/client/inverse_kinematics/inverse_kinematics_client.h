/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/spot/inverse_kinematics.pb.h>
#include <bosdyn/api/spot/inverse_kinematics_service.grpc.pb.h>
#include <bosdyn/api/spot/inverse_kinematics_service.pb.h>

#include <future>

#include "inverse_kinematics_error_codes.h"
#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/common/status.h"

namespace bosdyn {

namespace client {

// The return types for each RPC callback that provides the status of the RPC call in addition
// to the data/response message the RPC was returning from the robot.
typedef Result<::bosdyn::api::spot::InverseKinematicsResponse> InverseKinematicsResultType;

/**
 * The InverseKinematics service handles requests to a solution to an inverse kinematics problem for
 * Spot (or an indication that a solution could not be found). This creates a client which
 * communicates to the InverseKinematics service to request a solution to an inverse kinematics
 * problem.
 */
class InverseKinematicsClient : public ServiceClient {
 public:
    InverseKinematicsClient() = default;
    ~InverseKinematicsClient() = default;

    // Asynchronous method to request a solution to an inverse kinematics problem, which may not
    // exist.
    std::shared_future<InverseKinematicsResultType> InverseKinematicsAsync(
        ::bosdyn::api::spot::InverseKinematicsRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to request a solution to an inverse kinematics problem.
    InverseKinematicsResultType InverseKinematics(
        ::bosdyn::api::spot::InverseKinematicsRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    // Sets the QualityOfService enum for the inverse kinematics client to be used for network
    // selection optimization.
    QualityOfService GetQualityOfService() const override;

    // Set the communication details for the inverse kinematics service, including the stub and
    // processors for requests/responses.
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;

    // End of ServiceClient overrides.

    // Get the default service name the InverseKinematics service will be registered in the
    // directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the inverse kinematics service that will be registered in
    // the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous calls to request a solution to an inverse
    // kinematics problem.
    void OnInverseKinematicsComplete(MessagePumpCallBase* call,
                                     const ::bosdyn::api::spot::InverseKinematicsRequest& request,
                                     ::bosdyn::api::spot::InverseKinematicsResponse&& response,
                                     const grpc::Status& status,
                                     std::promise<InverseKinematicsResultType> promise);

    std::unique_ptr<::bosdyn::api::spot::InverseKinematicsService::Stub> m_stub;

    // Default service name for the inverse kinematics service.
    static const char* s_default_service_name;

    // Default service type for the inverse kinematics service.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
