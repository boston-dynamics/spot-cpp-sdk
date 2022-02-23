/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/fault_service.grpc.pb.h>
#include <bosdyn/api/fault_service.pb.h>
#include <bosdyn/api/service_fault.pb.h>

#include <future>

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/common/status.h"
#include "bosdyn/client/fault/fault_error_codes.h"

namespace bosdyn {

namespace client {

// The return types for each RPC callback that provides the status of the RPC call in addition
// to the data/response message the RPC was returning from the robot.
typedef Result<::bosdyn::api::TriggerServiceFaultResponse> TriggerServiceFaultResultType;
typedef Result<::bosdyn::api::ClearServiceFaultResponse> ClearServiceFaultResultType;

// Enum that defines if ClearServiceFault calls should clear a group of faults.
enum ServiceFaultClearGroup {
    CLEAR_SINGLE_FAULT,
    CLEAR_ALL_FOR_SERVICE_NAME,
    CLEAR_ALL_FOR_PAYLOAD_GUID,
};

/**
 * The Fault service enables off-board services to trigger and clear Service Faults that can
 * be accessed by any other service or client via the robot state.
 */
class FaultClient : public ServiceClient {
 public:
    // Constructor for the client.
    FaultClient() = default;

    // Destructor for the client.
    ~FaultClient() = default;

    // Asynchronous method to trigger a ServiceFault through the robot.
    std::shared_future<TriggerServiceFaultResultType> TriggerServiceFaultAsync(
        const ::bosdyn::api::ServiceFault& service_fault,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to trigger a ServiceFault through the robot.
    TriggerServiceFaultResultType TriggerServiceFault(
        const ::bosdyn::api::ServiceFault& service_fault,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to clear a ServiceFault from the robot.
    std::shared_future<ClearServiceFaultResultType> ClearServiceFaultAsync(
        const ::bosdyn::api::ServiceFaultId& fault_id,
        ServiceFaultClearGroup clear_group = ServiceFaultClearGroup::CLEAR_SINGLE_FAULT,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to clear a ServiceFault from the robot.
    ClearServiceFaultResultType ClearServiceFault(
        const ::bosdyn::api::ServiceFaultId& fault_id,
        ServiceFaultClearGroup clear_group = ServiceFaultClearGroup::CLEAR_SINGLE_FAULT,
        const RPCParameters& parameters = RPCParameters());

    // Sets the QualityOfService enum for the client to be used for network selection
    // optimization.
    QualityOfService GetQualityOfService() const override;

    // Set the communication details for the service, including the stub and processors
    // for requests/responses.
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;

    // Get the default service name the fault service will be registered in the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the fault service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // The stub for the client to communicate it's rpc with (and pass to the robot).
    std::unique_ptr<::bosdyn::api::FaultService::Stub> m_stub;

    // Default service name for the fault service.
    static const char* s_default_service_name;

    // Default service type for the fault service.
    static const char* s_service_type;

    // Callback for TriggerServiceFault.
    void OnTriggerServiceFaultComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::TriggerServiceFaultRequest& request,
        ::bosdyn::api::TriggerServiceFaultResponse&& response, const grpc::Status& status,
        std::promise<TriggerServiceFaultResultType> promise);

    // Callback for ClearServiceFault
    void OnClearServiceFaultComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::ClearServiceFaultRequest& request,
        ::bosdyn::api::ClearServiceFaultResponse&& response, const grpc::Status& status,
        std::promise<ClearServiceFaultResultType> promise);
};

}  // namespace client

}  // namespace bosdyn
