/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "estop_client.h"

#include "bosdyn/client/error_codes/rpc_error_code.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* EstopClient::s_default_service_name = "estop";

const char* EstopClient::s_service_type = "bosdyn.api.EstopService";

std::shared_future<RegisterEstopEndpointResultType> EstopClient::RegisterEstopEndpointAsync(
    ::bosdyn::api::RegisterEstopEndpointRequest& register_request,
    const RPCParameters& parameters) {
    std::promise<RegisterEstopEndpointResultType> response;
    std::shared_future<RegisterEstopEndpointResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::RegisterEstopEndpointRequest, ::bosdyn::api::RegisterEstopEndpointResponse,
                          ::bosdyn::api::RegisterEstopEndpointResponse>(
            register_request,
            std::bind(&::bosdyn::api::EstopService::Stub::AsyncRegisterEstopEndpoint, m_stub.get(), _1, _2,
                      _3),
            std::bind(&EstopClient::OnRegisterEstopEndpointComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

void EstopClient::OnRegisterEstopEndpointComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::RegisterEstopEndpointRequest& request,
    ::bosdyn::api::RegisterEstopEndpointResponse&& response, const grpc::Status& status,
    std::promise<RegisterEstopEndpointResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::RegisterEstopEndpointResponse>(
        status, response, response.status());

    promise.set_value({ret_status, std::move(response)});
}

RegisterEstopEndpointResultType EstopClient::RegisterEstopEndpoint(
    ::bosdyn::api::RegisterEstopEndpointRequest& register_request,
    const RPCParameters& parameters) {
    return RegisterEstopEndpointAsync(register_request, parameters).get();
}

std::shared_future<DeregisterEstopEndpointResultType> EstopClient::DeregisterEstopEndpointAsync(
    ::bosdyn::api::DeregisterEstopEndpointRequest& deregister_request,
    const RPCParameters& parameters) {
    std::promise<DeregisterEstopEndpointResultType> response;
    std::shared_future<DeregisterEstopEndpointResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::DeregisterEstopEndpointRequest, ::bosdyn::api::DeregisterEstopEndpointResponse,
                          ::bosdyn::api::DeregisterEstopEndpointResponse>(
            deregister_request,
            std::bind(&::bosdyn::api::EstopService::Stub::AsyncDeregisterEstopEndpoint, m_stub.get(), _1, _2,
                      _3),
            std::bind(&EstopClient::OnDeregisterEstopEndpointComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

void EstopClient::OnDeregisterEstopEndpointComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::DeregisterEstopEndpointRequest& request,
    ::bosdyn::api::DeregisterEstopEndpointResponse&& response, const grpc::Status& status,
    std::promise<DeregisterEstopEndpointResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::DeregisterEstopEndpointResponse>(
        status, response, response.status());

    promise.set_value({ret_status, std::move(response)});
}

DeregisterEstopEndpointResultType EstopClient::DeregisterEstopEndpoint(
    ::bosdyn::api::DeregisterEstopEndpointRequest& deregister_request,
    const RPCParameters& parameters) {
    return DeregisterEstopEndpointAsync(deregister_request, parameters).get();
}

std::shared_future<GetEstopConfigResultType> EstopClient::GetEstopConfigAsync(
    const std::string& target_config_id, const RPCParameters& parameters) {
    std::promise<GetEstopConfigResultType> response;
    std::shared_future<GetEstopConfigResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::GetEstopConfigRequest request;
    request.set_target_config_id(target_config_id);
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::GetEstopConfigRequest, ::bosdyn::api::GetEstopConfigResponse,
                          ::bosdyn::api::GetEstopConfigResponse>(
            request,
            std::bind(&::bosdyn::api::EstopService::Stub::AsyncGetEstopConfig, m_stub.get(), _1, _2, _3),
            std::bind(&EstopClient::OnGetEstopConfigComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

void EstopClient::OnGetEstopConfigComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::GetEstopConfigRequest& request,
    ::bosdyn::api::GetEstopConfigResponse&& response, const grpc::Status& status,
    std::promise<GetEstopConfigResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::GetEstopConfigResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

GetEstopConfigResultType EstopClient::GetEstopConfig(
    const std::string& target_config_id, const RPCParameters& parameters) {
    return GetEstopConfigAsync(target_config_id, parameters).get();
}

std::shared_future<SetEstopConfigResultType> EstopClient::SetEstopConfigAsync(
    ::bosdyn::api::EstopConfig& estop_config, const std::string& target_config_id,
    const RPCParameters& parameters) {
    std::promise<SetEstopConfigResultType> response;
    std::shared_future<SetEstopConfigResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::SetEstopConfigRequest request;
    request.set_target_config_id(target_config_id);
    request.mutable_config()->CopyFrom(estop_config);
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::SetEstopConfigRequest, ::bosdyn::api::SetEstopConfigResponse,
                          ::bosdyn::api::SetEstopConfigResponse>(
            request,
            std::bind(&::bosdyn::api::EstopService::Stub::AsyncSetEstopConfig, m_stub.get(), _1, _2, _3),
            std::bind(&EstopClient::OnSetEstopConfigComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

void EstopClient::OnSetEstopConfigComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::SetEstopConfigRequest& request,
    ::bosdyn::api::SetEstopConfigResponse&& response, const grpc::Status& status,
    std::promise<SetEstopConfigResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::SetEstopConfigResponse>(
        status, response, response.status());

    promise.set_value({ret_status, std::move(response)});
}

SetEstopConfigResultType EstopClient::SetEstopConfig(
    ::bosdyn::api::EstopConfig& estop_config, const std::string& target_config_id,
    const RPCParameters& parameters) {
    return SetEstopConfigAsync(estop_config, target_config_id, parameters).get();
}

std::shared_future<GetEstopSystemStatusResultType> EstopClient::GetEstopStatusAsync(
    const RPCParameters& parameters) {
    std::promise<GetEstopSystemStatusResultType> response;
    std::shared_future<GetEstopSystemStatusResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::GetEstopSystemStatusRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::GetEstopSystemStatusRequest, ::bosdyn::api::GetEstopSystemStatusResponse,
                          ::bosdyn::api::GetEstopSystemStatusResponse>(
            request,
            std::bind(&::bosdyn::api::EstopService::Stub::AsyncGetEstopSystemStatus, m_stub.get(), _1, _2,
                      _3),
            std::bind(&EstopClient::OnGetEstopStatusComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

void EstopClient::OnGetEstopStatusComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::GetEstopSystemStatusRequest& request,
    ::bosdyn::api::GetEstopSystemStatusResponse&& response, const grpc::Status& status,
    std::promise<GetEstopSystemStatusResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::GetEstopSystemStatusResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

GetEstopSystemStatusResultType EstopClient::GetEstopStatus(
    const RPCParameters& parameters) {
    return GetEstopStatusAsync(parameters).get();
}

std::shared_future<EstopCheckInResultType> EstopClient::EstopCheckInAsync(
    ::bosdyn::api::EstopCheckInRequest& request, const RPCParameters& parameters) {
    std::promise<EstopCheckInResultType> response;
    std::shared_future<EstopCheckInResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::EstopCheckInRequest, ::bosdyn::api::EstopCheckInResponse,
                          ::bosdyn::api::EstopCheckInResponse>(
            request,
            std::bind(&::bosdyn::api::EstopService::Stub::AsyncEstopCheckIn, m_stub.get(), _1, _2, _3),
            std::bind(&EstopClient::OnEstopCheckInComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

void EstopClient::OnEstopCheckInComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::EstopCheckInRequest& request,
    ::bosdyn::api::EstopCheckInResponse&& response, const grpc::Status& status,
    std::promise<EstopCheckInResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::EstopCheckInResponse>(
        status, response, response.status());

    promise.set_value({ret_status, std::move(response)});
}

EstopCheckInResultType EstopClient::EstopCheckIn(
    ::bosdyn::api::EstopCheckInRequest& request, const RPCParameters& parameters) {
    return EstopCheckInAsync(request, parameters).get();
}

ServiceClient::QualityOfService EstopClient::GetQualityOfService() const {
    return QualityOfService::LATENCY_CRITICAL;
}

void EstopClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::EstopService::Stub(channel));
}

}  // namespace client

}  // namespace bosdyn
