/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "fault_client.h"

#include "bosdyn/client/error_codes/sdk_error_code.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* FaultClient::s_default_service_name = "fault";

const char* FaultClient::s_service_type = "bosdyn.api.FaultService";

// Asynchronous method to trigger a ServiceFault.
std::shared_future<TriggerServiceFaultResultType> FaultClient::TriggerServiceFaultAsync(
    const ::bosdyn::api::ServiceFault& service_fault, const RPCParameters& parameters) {
    std::promise<TriggerServiceFaultResultType> response;
    std::shared_future<TriggerServiceFaultResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::TriggerServiceFaultRequest request;
    *request.mutable_fault() = service_fault;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::TriggerServiceFaultRequest, ::bosdyn::api::TriggerServiceFaultResponse,
                          ::bosdyn::api::TriggerServiceFaultResponse>(
            request,
            std::bind(&::bosdyn::api::FaultService::Stub::AsyncTriggerServiceFault, m_stub.get(), _1, _2, _3),
            std::bind(&FaultClient::OnTriggerServiceFaultComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

// Callback for TriggerServiceFault.
void FaultClient::OnTriggerServiceFaultComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::TriggerServiceFaultRequest& request,
    ::bosdyn::api::TriggerServiceFaultResponse&& response, const grpc::Status& status,
    std::promise<TriggerServiceFaultResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::TriggerServiceFaultResponse>(
        status, response, response.status());

    promise.set_value({ret_status, std::move(response)});
}

// Synchronous method to trigger a ServiceFault through the robot.
TriggerServiceFaultResultType FaultClient::TriggerServiceFault(
    const ::bosdyn::api::ServiceFault& service_fault,
    const RPCParameters& parameters) {
    return this->TriggerServiceFaultAsync(service_fault, parameters).get();
}

// Asynchronous method to clear a ServiceFault from the robot.
std::shared_future<ClearServiceFaultResultType> FaultClient::ClearServiceFaultAsync(
    const ::bosdyn::api::ServiceFaultId& fault_id, ServiceFaultClearGroup clear_group,
    const RPCParameters& parameters) {
    std::promise<ClearServiceFaultResultType> response;
    std::shared_future<ClearServiceFaultResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::ClearServiceFaultRequest request;
    *request.mutable_fault_id() = fault_id;
    if (clear_group == CLEAR_ALL_FOR_SERVICE_NAME) {
        request.set_clear_all_service_faults(true);
    } else if (clear_group == CLEAR_ALL_FOR_PAYLOAD_GUID) {
        request.set_clear_all_payload_faults(true);
    }
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::ClearServiceFaultRequest, ::bosdyn::api::ClearServiceFaultResponse,
                          ::bosdyn::api::ClearServiceFaultResponse>(
            request,
            std::bind(&::bosdyn::api::FaultService::Stub::AsyncClearServiceFault, m_stub.get(), _1, _2, _3),
            std::bind(&FaultClient::OnClearServiceFaultComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

// Callback for ClearServiceFault.
void FaultClient::OnClearServiceFaultComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::ClearServiceFaultRequest& request,
    ::bosdyn::api::ClearServiceFaultResponse&& response, const grpc::Status& status,
    std::promise<ClearServiceFaultResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::ClearServiceFaultResponse>(
        status, response, response.status());

    promise.set_value({ret_status, std::move(response)});
}

// Synchronous method to clear a ServiceFault from the robot.
ClearServiceFaultResultType FaultClient::ClearServiceFault(
    const ::bosdyn::api::ServiceFaultId& fault_id, ServiceFaultClearGroup clear_group,
    const RPCParameters& parameters) {
    return ClearServiceFaultAsync(fault_id, clear_group, parameters).get();
}

ServiceClient::QualityOfService FaultClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void FaultClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::FaultService::Stub(channel));
}

}  // namespace client

}  // namespace bosdyn
