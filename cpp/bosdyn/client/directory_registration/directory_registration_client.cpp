/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/directory_registration/directory_registration_client.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* DirectoryRegistrationClient::s_default_service_name = "directory-registration";

const char* DirectoryRegistrationClient::s_default_service_authority = "api.spot.robot";

const char* DirectoryRegistrationClient::s_service_type = "bosdyn.api.DirectoryRegistrationService";

std::shared_future<RegisterServiceResultType> DirectoryRegistrationClient::RegisterServiceAsync(
    ::bosdyn::api::RegisterServiceRequest& request,
    const RPCParameters& parameters) {
    std::promise<RegisterServiceResultType> response;
    std::shared_future<RegisterServiceResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::RegisterServiceRequest, ::bosdyn::api::RegisterServiceResponse,
                          ::bosdyn::api::RegisterServiceResponse>(
            request,
            std::bind(&::bosdyn::api::DirectoryRegistrationService::Stub::AsyncRegisterService, m_stub.get(),
                      _1, _2, _3),
            std::bind(&DirectoryRegistrationClient::OnRegisterServiceComplete, this, _1, _2, _3, _4,
                      _5),
            std::move(response), parameters);

    return future;
}

void DirectoryRegistrationClient::OnRegisterServiceComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::RegisterServiceRequest& request,
    ::bosdyn::api::RegisterServiceResponse&& response, const grpc::Status& status,
    std::promise<RegisterServiceResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::RegisterServiceResponse>(
        status, response, response.status());

    promise.set_value({ret_status, std::move(response)});
}

RegisterServiceResultType DirectoryRegistrationClient::RegisterService(
    ::bosdyn::api::RegisterServiceRequest& request,
    const RPCParameters& parameters) {
    return RegisterServiceAsync(request, parameters).get();
}

std::shared_future<UnregisterServiceResultType> DirectoryRegistrationClient::UnregisterServiceAsync(
    const std::string& service_name, const RPCParameters& parameters) {
    std::promise<UnregisterServiceResultType> response;
    std::shared_future<UnregisterServiceResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::UnregisterServiceRequest request;
    request.set_service_name(service_name);
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::UnregisterServiceRequest, ::bosdyn::api::UnregisterServiceResponse,
                          ::bosdyn::api::UnregisterServiceResponse>(
            request,
            std::bind(&::bosdyn::api::DirectoryRegistrationService::Stub::AsyncUnregisterService,
                      m_stub.get(), _1, _2, _3),
            std::bind(&DirectoryRegistrationClient::OnUnregisterServiceComplete, this, _1, _2, _3,
                      _4, _5),
            std::move(response), parameters);

    return future;
}

void DirectoryRegistrationClient::OnUnregisterServiceComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::UnregisterServiceRequest& request,
    ::bosdyn::api::UnregisterServiceResponse&& response, const grpc::Status& status,
    std::promise<UnregisterServiceResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::UnregisterServiceResponse>(
        status, response, response.status());

    promise.set_value({ret_status, std::move(response)});
}

UnregisterServiceResultType DirectoryRegistrationClient::UnregisterService(
    const std::string& service_name, const RPCParameters& parameters) {
    return UnregisterServiceAsync(service_name, parameters).get();
}

std::shared_future<UpdateServiceResultType> DirectoryRegistrationClient::UpdateServiceAsync(
    ::bosdyn::api::UpdateServiceRequest& request, const RPCParameters& parameters) {
    std::promise<UpdateServiceResultType> response;
    std::shared_future<UpdateServiceResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::UpdateServiceRequest, ::bosdyn::api::UpdateServiceResponse,
                          ::bosdyn::api::UpdateServiceResponse>(
            request,
            std::bind(&::bosdyn::api::DirectoryRegistrationService::Stub::AsyncUpdateService, m_stub.get(),
                      _1, _2, _3),
            std::bind(&DirectoryRegistrationClient::OnUpdateServiceComplete, this, _1, _2, _3, _4,
                      _5),
            std::move(response), parameters);

    return future;
}

void DirectoryRegistrationClient::OnUpdateServiceComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::UpdateServiceRequest& request,
    ::bosdyn::api::UpdateServiceResponse&& response, const grpc::Status& status,
    std::promise<UpdateServiceResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::UpdateServiceResponse>(
        status, response, response.status());

    promise.set_value({ret_status, std::move(response)});
}

UpdateServiceResultType DirectoryRegistrationClient::UpdateService(
    ::bosdyn::api::UpdateServiceRequest& request, const RPCParameters& parameters) {
    return UpdateServiceAsync(request, parameters).get();
}

ServiceClient::QualityOfService DirectoryRegistrationClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void DirectoryRegistrationClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::DirectoryRegistrationService::Stub(channel));
}
// End of ServiceClient overrides.

}  // namespace client

}  // namespace bosdyn
