/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "payload_registration_client.h"
#include "bosdyn/client/error_codes/rpc_error_code.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"

#include <bosdyn/api/robot_id.pb.h>

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* PayloadRegistrationClient::s_default_service_name = "payload-registration";

const char* PayloadRegistrationClient::s_default_service_authority =
    "payload-registration.spot.robot";

const char* PayloadRegistrationClient::s_service_type = "bosdyn.api.PayloadRegistrationService";

std::shared_future<RegisterPayloadResultType> PayloadRegistrationClient::RegisterPayloadAsync(
    ::bosdyn::api::RegisterPayloadRequest& request,
    const RPCParameters& parameters) {
    std::promise<RegisterPayloadResultType> response;
    std::shared_future<RegisterPayloadResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::RegisterPayloadRequest, ::bosdyn::api::RegisterPayloadResponse,
                          ::bosdyn::api::RegisterPayloadResponse>(
            request,
            std::bind(&::bosdyn::api::PayloadRegistrationService::Stub::AsyncRegisterPayload, m_stub.get(),
                      _1, _2, _3),
            std::bind(&PayloadRegistrationClient::OnRegisterPayloadComplete, this, _1, _2, _3, _4,
                      _5),
            std::move(response), parameters);

    return future;
}

void PayloadRegistrationClient::OnRegisterPayloadComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::RegisterPayloadRequest& request,
    ::bosdyn::api::RegisterPayloadResponse&& response, const grpc::Status& status,
    std::promise<RegisterPayloadResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::RegisterPayloadResponse>(
        status, response, response.status());

    promise.set_value({ret_status, std::move(response)});
}

RegisterPayloadResultType PayloadRegistrationClient::RegisterPayload(
    ::bosdyn::api::RegisterPayloadRequest& request,
    const RPCParameters& parameters) {
    return RegisterPayloadAsync(request, parameters).get();
}

std::shared_future<UpdatePayloadVersionResultType>
PayloadRegistrationClient::UpdatePayloadVersionAsync(
    ::bosdyn::api::UpdatePayloadVersionRequest& request,
    const RPCParameters& parameters) {
    std::promise<UpdatePayloadVersionResultType> response;
    std::shared_future<UpdatePayloadVersionResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::UpdatePayloadVersionRequest, ::bosdyn::api::UpdatePayloadVersionResponse,
                          ::bosdyn::api::UpdatePayloadVersionResponse>(
            request,
            std::bind(&::bosdyn::api::PayloadRegistrationService::Stub::AsyncUpdatePayloadVersion,
                      m_stub.get(), _1, _2, _3),
            std::bind(&PayloadRegistrationClient::OnUpdatePayloadVersionComplete, this, _1, _2, _3,
                      _4, _5),
            std::move(response), parameters);

    return future;
}

void PayloadRegistrationClient::OnUpdatePayloadVersionComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::UpdatePayloadVersionRequest& request,
    ::bosdyn::api::UpdatePayloadVersionResponse&& response, const grpc::Status& status,
    std::promise<UpdatePayloadVersionResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::UpdatePayloadVersionResponse>(
        status, response, response.status());

    promise.set_value({ret_status, std::move(response)});
}

UpdatePayloadVersionResultType PayloadRegistrationClient::UpdatePayloadVersion(
    ::bosdyn::api::UpdatePayloadVersionRequest& request,
    const RPCParameters& parameters) {
    return UpdatePayloadVersionAsync(request, parameters).get();
}

std::shared_future<UpdatePayloadVersionResultType>
PayloadRegistrationClient::UpdatePayloadVersionAsync(
    const std::string& guid, const std::string& secret, int major_version, int minor_version,
    int patch_level, const RPCParameters& parameters) {
    ::bosdyn::api::UpdatePayloadVersionRequest request;
    // Deprecated credential fields.
    request.set_payload_guid(guid);
    request.set_payload_secret(secret);
    // Supported credential fields for 2.4+ robots.
    request.mutable_payload_credentials()->set_guid(guid);
    request.mutable_payload_credentials()->set_secret(secret);
    ::bosdyn::api::SoftwareVersion* version = request.mutable_updated_version();
    version->set_major_version(major_version);
    version->set_minor_version(minor_version);
    version->set_patch_level(patch_level);
    return UpdatePayloadVersionAsync(request, parameters);
}

UpdatePayloadVersionResultType
PayloadRegistrationClient::PayloadRegistrationClient::UpdatePayloadVersion(
    const std::string& guid, const std::string& secret, int major_version, int minor_version,
    int patch_level, const RPCParameters& parameters) {
    return UpdatePayloadVersionAsync(
        guid, secret, major_version, minor_version, patch_level, parameters).get();
}

std::shared_future<GetPayloadAuthTokenResultType>
PayloadRegistrationClient::GetPayloadAuthTokenAsync(
    ::bosdyn::api::GetPayloadAuthTokenRequest& request,
    const RPCParameters& parameters) {
    std::promise<GetPayloadAuthTokenResultType> response;
    std::shared_future<GetPayloadAuthTokenResultType> future = response.get_future();

    if (!m_stub) {
        response.set_value({::bosdyn::common::Status(SDKErrorCode::GenericSDKError,
                                   "Stub is not set in PayloadRegistrationClient Client"),
                            {}});
        return future;
    }

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::GetPayloadAuthTokenRequest, ::bosdyn::api::GetPayloadAuthTokenResponse,
                          ::bosdyn::api::GetPayloadAuthTokenResponse>(
            request,
            std::bind(&::bosdyn::api::PayloadRegistrationService::Stub::AsyncGetPayloadAuthToken,
                      m_stub.get(), _1, _2, _3),
            std::bind(&PayloadRegistrationClient::OnGetPayloadAuthTokenComplete, this, _1, _2, _3,
                      _4, _5),
            std::move(response), parameters);

    return future;
}

void PayloadRegistrationClient::OnGetPayloadAuthTokenComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::GetPayloadAuthTokenRequest& request,
    ::bosdyn::api::GetPayloadAuthTokenResponse&& response, const grpc::Status& status,
    std::promise<GetPayloadAuthTokenResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::GetPayloadAuthTokenResponse>(
        status, response, response.status());

    promise.set_value({ret_status, std::move(response)});
}

GetPayloadAuthTokenResultType PayloadRegistrationClient::GetPayloadAuthToken(
    ::bosdyn::api::GetPayloadAuthTokenRequest& request,
    const RPCParameters& parameters) {
    return GetPayloadAuthTokenAsync(request, parameters).get();
}

std::shared_future<GetPayloadAuthTokenResultType>
PayloadRegistrationClient::GetPayloadAuthTokenAsync(
    const std::string& guid, const std::string& secret,
    const RPCParameters& parameters) {
    ::bosdyn::api::GetPayloadAuthTokenRequest request;
    // Deprecated credential fields.
    request.set_payload_guid(guid);
    request.set_payload_secret(secret);
    // Supported credential fields for 2.4+ robots.
    request.mutable_payload_credentials()->set_guid(guid);
    request.mutable_payload_credentials()->set_secret(secret);
    return GetPayloadAuthTokenAsync(request, parameters);
}

GetPayloadAuthTokenResultType PayloadRegistrationClient::GetPayloadAuthToken(
    const std::string& guid, const std::string& secret,
    const RPCParameters& parameters) {
    return GetPayloadAuthTokenAsync(guid, secret, parameters).get();
}

std::shared_future<UpdatePayloadAttachedResultType>
PayloadRegistrationClient::UpdatePayloadAttachedAsync(
    ::bosdyn::api::UpdatePayloadAttachedRequest& request,
    const RPCParameters& parameters) {
    std::promise<UpdatePayloadAttachedResultType> response;
    std::shared_future<UpdatePayloadAttachedResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::UpdatePayloadAttachedRequest, ::bosdyn::api::UpdatePayloadAttachedResponse,
                          ::bosdyn::api::UpdatePayloadAttachedResponse>(
            request,
            std::bind(&::bosdyn::api::PayloadRegistrationService::Stub::AsyncUpdatePayloadAttached,
                      m_stub.get(), _1, _2, _3),
            std::bind(&PayloadRegistrationClient::OnUpdatePayloadAttachedComplete, this, _1, _2, _3,
                      _4, _5),
            std::move(response), parameters);

    return future;
}

void PayloadRegistrationClient::OnUpdatePayloadAttachedComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::UpdatePayloadAttachedRequest& request,
    ::bosdyn::api::UpdatePayloadAttachedResponse&& response, const grpc::Status& status,
    std::promise<UpdatePayloadAttachedResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::UpdatePayloadAttachedResponse>(
        status, response, response.status());

    promise.set_value({ret_status, std::move(response)});
}

UpdatePayloadAttachedResultType PayloadRegistrationClient::UpdatePayloadAttached(
    ::bosdyn::api::UpdatePayloadAttachedRequest& request,
    const RPCParameters& parameters) {
    return UpdatePayloadAttachedAsync(request, parameters).get();
}

std::shared_future<UpdatePayloadAttachedResultType> PayloadRegistrationClient::AttachPayloadAsync(
    const std::string& guid, const std::string& secret, const RPCParameters& parameters) {
    auto request = MakeAttachDetachRequest(guid, secret, true);
    return UpdatePayloadAttachedAsync(request, parameters);
}

UpdatePayloadAttachedResultType PayloadRegistrationClient::AttachPayload(
    const std::string& guid, const std::string& secret, const RPCParameters& parameters) {
    return AttachPayloadAsync(guid, secret, parameters).get();
}

std::shared_future<UpdatePayloadAttachedResultType> PayloadRegistrationClient::DetachPayloadAsync(
    const std::string& guid, const std::string& secret, const RPCParameters& parameters) {
    auto request = MakeAttachDetachRequest(guid, secret, false);
    return UpdatePayloadAttachedAsync(request, parameters);
}

UpdatePayloadAttachedResultType PayloadRegistrationClient::DetachPayload(
    const std::string& guid, const std::string& secret, const RPCParameters& parameters) {
    return DetachPayloadAsync(guid, secret, parameters).get();
}

ServiceClient::QualityOfService PayloadRegistrationClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void PayloadRegistrationClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::PayloadRegistrationService::Stub(channel));
}

::bosdyn::api::UpdatePayloadAttachedRequest PayloadRegistrationClient::MakeAttachDetachRequest(
    const std::string& guid, const std::string& secret, const bool& attach) {
    ::bosdyn::api::UpdatePayloadAttachedRequest request;
    request.mutable_payload_credentials()->set_guid(guid);
    request.mutable_payload_credentials()->set_secret(secret);
    request.set_request(attach ? ::bosdyn::api::UpdatePayloadAttachedRequest_Request_REQUEST_ATTACH : ::bosdyn::api::UpdatePayloadAttachedRequest_Request_REQUEST_DETACH);
    return request;
}

}  // namespace client

}  // namespace bosdyn
