/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/client/directory_registration/directory_registration_error_codes.h"

#include <bosdyn/api/directory_registration_service.grpc.pb.h>
#include <bosdyn/api/directory_registration_service.pb.h>
#include <future>

namespace bosdyn {

namespace client {

typedef Result<::bosdyn::api::RegisterServiceResponse> RegisterServiceResultType;
typedef Result<::bosdyn::api::UnregisterServiceResponse> UnregisterServiceResultType;
typedef Result<::bosdyn::api::UpdateServiceResponse> UpdateServiceResultType;

class DirectoryRegistrationClient : public ServiceClient {
 public:
    DirectoryRegistrationClient() = default;

    ~DirectoryRegistrationClient() = default;

    // Asynchronous method to register a service.
    std::shared_future<RegisterServiceResultType> RegisterServiceAsync(
        ::bosdyn::api::RegisterServiceRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to register a service.
    RegisterServiceResultType RegisterService(
        ::bosdyn::api::RegisterServiceRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to unregister a service.
    std::shared_future<UnregisterServiceResultType> UnregisterServiceAsync(
        const std::string& service_name,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to unregister a service.
    UnregisterServiceResultType UnregisterService(
        const std::string& service_name,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to update a service.
    std::shared_future<UpdateServiceResultType> UpdateServiceAsync(
        ::bosdyn::api::UpdateServiceRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to update a service.
    UpdateServiceResultType UpdateService(
        ::bosdyn::api::UpdateServiceRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the DirectoryRegistration service will be registered in the
    // directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service authority the DirectoryRegistration service will be registered in the
    // directory with.
    static std::string GetDefaultServiceAuthority() { return s_default_service_authority; }

    // Get the default service type for the DirectoryRegistration service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous calls to register a service.
    void OnRegisterServiceComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::RegisterServiceRequest& request,
        ::bosdyn::api::RegisterServiceResponse&& response, const grpc::Status& status,
        std::promise<RegisterServiceResultType> promise);

    // Callback function registered for the asynchronous calls to unregister a service.
    void OnUnregisterServiceComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::UnregisterServiceRequest& request,
        ::bosdyn::api::UnregisterServiceResponse&& response, const grpc::Status& status,
        std::promise<UnregisterServiceResultType> promise);

    // Callback function registered for the asynchronous calls to update a service.
    void OnUpdateServiceComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::UpdateServiceRequest& request,
        ::bosdyn::api::UpdateServiceResponse&& response, const grpc::Status& status,
        std::promise<UpdateServiceResultType> promise);

    std::unique_ptr<::bosdyn::api::DirectoryRegistrationService::Stub> m_stub;

    // Default service name for the DirectoryRegistration service.
    static const char* s_default_service_name;

    // Default service authority for the DirectoryRegistration service.
    static const char* s_default_service_authority;

    // Default service type for the DirectoryRegistration service.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
