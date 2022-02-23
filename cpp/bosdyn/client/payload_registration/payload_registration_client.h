/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/payload_registration.pb.h>
#include <bosdyn/api/payload_registration_service.grpc.pb.h>
#include <bosdyn/api/payload_registration_service.pb.h>

#include "payload_registration_error_codes.h"
#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/common/status.h"

#include <future>

namespace bosdyn {

namespace client {

typedef Result<::bosdyn::api::RegisterPayloadResponse> RegisterPayloadResultType;
typedef Result<::bosdyn::api::UpdatePayloadVersionResponse> UpdatePayloadVersionResultType;
typedef Result<::bosdyn::api::GetPayloadAuthTokenResponse> GetPayloadAuthTokenResultType;
typedef Result<::bosdyn::api::UpdatePayloadAttachedResponse> UpdatePayloadAttachedResultType;

class PayloadRegistrationClient : public ServiceClient {
 public:
    PayloadRegistrationClient() = default;

    ~PayloadRegistrationClient() = default;

    // Asynchronous method to register a payload.
    std::shared_future<RegisterPayloadResultType> RegisterPayloadAsync(
        ::bosdyn::api::RegisterPayloadRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to register a payload.
    RegisterPayloadResultType RegisterPayload(
        ::bosdyn::api::RegisterPayloadRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to update a payload version.
    std::shared_future<UpdatePayloadVersionResultType> UpdatePayloadVersionAsync(
        ::bosdyn::api::UpdatePayloadVersionRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to update a payload version.
    UpdatePayloadVersionResultType UpdatePayloadVersion(
        ::bosdyn::api::UpdatePayloadVersionRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to request a payload auth token.
    std::shared_future<GetPayloadAuthTokenResultType> GetPayloadAuthTokenAsync(
        ::bosdyn::api::GetPayloadAuthTokenRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to unregister a payload auth token.
    GetPayloadAuthTokenResultType GetPayloadAuthToken(
        ::bosdyn::api::GetPayloadAuthTokenRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to attach or detach the identified payload.
    std::shared_future<UpdatePayloadAttachedResultType> UpdatePayloadAttachedAsync(
        ::bosdyn::api::UpdatePayloadAttachedRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to attach or detach the identified payload.
    UpdatePayloadAttachedResultType UpdatePayloadAttached(
        ::bosdyn::api::UpdatePayloadAttachedRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to attach the identified payload.
    std::shared_future<UpdatePayloadAttachedResultType> AttachPayloadAsync(
        const std::string& guid, const std::string& secret, const RPCParameters& parameters = RPCParameters());

    // Synchronous method to attach the identified payload.
    UpdatePayloadAttachedResultType AttachPayload(
        const std::string& guid, const std::string& secret, const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to detach the identified payload.
    std::shared_future<UpdatePayloadAttachedResultType> DetachPayloadAsync(
        const std::string& guid, const std::string& secret, const RPCParameters& parameters = RPCParameters());

    // Synchronous method to detach the identified payload.
    UpdatePayloadAttachedResultType DetachPayload(
        const std::string& guid, const std::string& secret, const RPCParameters& parameters = RPCParameters());

    // Start of helper methods.
    // Update payload version helpers.
    // guid is the guid of the payload to be updated.
    // secret is the secret of the payload to be updated.
    // major_version is the major version (first number) to set the payload at.
    // minor_version is the minor version (second number) to set the payload at.
    // patch_level is the patch level (third number) to set the payload at.
    std::shared_future<UpdatePayloadVersionResultType> UpdatePayloadVersionAsync(
        const std::string& guid, const std::string& secret, int major_version, int minor_version,
        int patch_level, const RPCParameters& parameters = RPCParameters());
    UpdatePayloadVersionResultType UpdatePayloadVersion(
        const std::string& guid, const std::string& secret, int major_version, int minor_version,
        int patch_level, const RPCParameters& parameters = RPCParameters());

    // Get payload auth token helpers.
    // guid is the guid of the payload requesting a token.
    // secret is the secret of the payload requesting a token.
    std::shared_future<GetPayloadAuthTokenResultType> GetPayloadAuthTokenAsync(
        const std::string& guid, const std::string& secret,
        const RPCParameters& parameters = RPCParameters());
    GetPayloadAuthTokenResultType GetPayloadAuthToken(
        const std::string& guid, const std::string& secret,
        const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the PayloadRegistration service will be registered in the
    // directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service authority the PayloadRegistration service will be registered in the
    // directory with.
    static std::string GetDefaultServiceAuthority() { return s_default_service_authority; }

    // Get the default service type for the PayloadRegistration service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous calls to register a payload.
    void OnRegisterPayloadComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::RegisterPayloadRequest& request,
        ::bosdyn::api::RegisterPayloadResponse&& response, const grpc::Status& status,
        std::promise<RegisterPayloadResultType> promise);

    // Callback function registered for the asynchronous calls to update a payload version.
    void OnUpdatePayloadVersionComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::UpdatePayloadVersionRequest& request,
        ::bosdyn::api::UpdatePayloadVersionResponse&& response, const grpc::Status& status,
        std::promise<UpdatePayloadVersionResultType> promise);

    // Callback function registered for the asynchronous calls to get a payload auth token.
    void OnGetPayloadAuthTokenComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::GetPayloadAuthTokenRequest& request,
        ::bosdyn::api::GetPayloadAuthTokenResponse&& response, const grpc::Status& status,
        std::promise<GetPayloadAuthTokenResultType> promise);

    // Callback function registered for the asynchronous calls to attach/detach a payload.
    void OnUpdatePayloadAttachedComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::UpdatePayloadAttachedRequest& request,
        ::bosdyn::api::UpdatePayloadAttachedResponse&& response, const grpc::Status& status,
        std::promise<UpdatePayloadAttachedResultType> promise);

    ::bosdyn::api::UpdatePayloadAttachedRequest MakeAttachDetachRequest(const std::string& guid,
                                                              const std::string& secret,
                                                              const bool& attach);

    std::unique_ptr<::bosdyn::api::PayloadRegistrationService::Stub> m_stub;

    // Default service name for the PayloadRegistration service.
    static const char* s_default_service_name;

    // Default service authority for the PayloadRegistration service.
    static const char* s_default_service_authority;

    // Default service type for the PayloadRegistration service.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
