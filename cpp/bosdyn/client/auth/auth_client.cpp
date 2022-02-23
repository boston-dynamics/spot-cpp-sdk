/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/auth/auth_client.h"
#include "bosdyn/client/error_codes/proto_enum_to_stderror_macro.h"
#include "bosdyn/client/error_codes/rpc_error_code.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* AuthClient::s_default_service_name = "auth";

const char* AuthClient::s_default_service_authority = "auth.spot.robot";

const char* AuthClient::s_service_type = "bosdyn.api.AuthService";

std::shared_future<AuthResultType> AuthClient::GetAuthTokenAsync(
    const std::string& username, const std::string& password,
    const RPCParameters& parameters) {
    std::promise<AuthResultType> response;
    std::shared_future<AuthResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::GetAuthTokenRequest request;
    request.set_username(username);
    request.set_password(password);

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::GetAuthTokenRequest, ::bosdyn::api::GetAuthTokenResponse, ::bosdyn::api::GetAuthTokenResponse>(
            request,
            std::bind(&::bosdyn::api::AuthService::Stub::AsyncGetAuthToken, m_stub.get(), _1, _2, _3),
            std::bind(&AuthClient::OnGetAuthTokenComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);
    return future;
}

std::shared_future<AuthResultType> AuthClient::GetAuthTokenAsync(
    const std::string& token, const RPCParameters& parameters) {
    std::promise<AuthResultType> response;
    std::shared_future<AuthResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::GetAuthTokenRequest request;
    request.set_token(token);

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::GetAuthTokenRequest, ::bosdyn::api::GetAuthTokenResponse, ::bosdyn::api::GetAuthTokenResponse>(
            request,
            std::bind(&::bosdyn::api::AuthService::Stub::AsyncGetAuthToken, m_stub.get(), _1, _2, _3),
            std::bind(&AuthClient::OnGetAuthTokenComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

AuthResultType AuthClient::GetAuthToken(
    const std::string& username, const std::string& password,
    const RPCParameters& parameters) {
    return GetAuthTokenAsync(username, password, parameters).get();
}

AuthResultType AuthClient::GetAuthToken(
    const std::string& token, const RPCParameters& parameters) {
    return GetAuthTokenAsync(token, parameters).get();
}

// Set the outstanding response, which will update any futures. Reset it.
void AuthClient::OnGetAuthTokenComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::GetAuthTokenRequest& request,
    ::bosdyn::api::GetAuthTokenResponse&& response, const grpc::Status& status,
    std::promise<AuthResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::GetAuthTokenResponse>(
        status, response, response.status());
    if (!ret_status) {
        promise.set_value({ret_status, std::move(response)});
        return;
    }

    promise.set_value({ret_status, std::move(response)});
}

// Start of ServiceClient overrides.
ServiceClient::QualityOfService AuthClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void AuthClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::AuthService::Stub(channel));
}
// End of ServiceClient overrides.

}  // namespace client

}  // namespace bosdyn
