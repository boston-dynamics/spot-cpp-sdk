/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include "bosdyn/client/auth/auth_error_codes.h"
#include "bosdyn/client/service_client/service_client.h"

#include <bosdyn/api/auth_service.grpc.pb.h>
#include <bosdyn/api/auth_service.pb.h>
#include <future>

namespace bosdyn {

namespace client {

// This typedef needs to be a std::shared_ptr to satisfy the InitiateCall templatized method in
// ServiceClient
    typedef Result<::bosdyn::api::GetAuthTokenResponse> AuthResultType;

class AuthClient : public ServiceClient {
 public:
    AuthClient() = default;

    ~AuthClient() = default;

    // Asynchronous method to get an auth token for username/password combination.
    std::shared_future<AuthResultType> GetAuthTokenAsync(
        const std::string& username, const std::string& password,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to get an auth token for username/password combination.
    AuthResultType GetAuthToken(
        const std::string& username, const std::string& password,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to get an auth token for provided token.
    std::shared_future<AuthResultType> GetAuthTokenAsync(
        const std::string& token, const RPCParameters& parameters = RPCParameters());

    // Synchronous method to get an auth token for provided token.
    AuthResultType GetAuthToken(
        const std::string& token, const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // End of ServiceClient overrides.

    // Get the default service name the Auth service will be registered in the directory
    // with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service authority the Auth service will be registered in the directory
    // with.
    static std::string GetDefaultServiceAuthority() { return s_default_service_authority; }

    // Get the default service type for the Auth service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    void OnGetAuthTokenComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::GetAuthTokenRequest& request,
        ::bosdyn::api::GetAuthTokenResponse&& response, const grpc::Status& status,
        std::promise<AuthResultType> promise);

    std::unique_ptr<::bosdyn::api::AuthService::Stub> m_stub;

    // Default service name for the Auth service.
    static const char* s_default_service_name;

    // Default service authority for the Auth service.
    static const char* s_default_service_authority;

    // Default service type for the AuthService.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
