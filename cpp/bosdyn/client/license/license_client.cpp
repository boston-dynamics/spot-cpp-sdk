/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/license/license_client.h"
#include "bosdyn/common/time.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* LicenseClient::s_default_service_name = "license";

const char* LicenseClient::s_service_type = "bosdyn.api.LicenseService";

std::shared_future<GetLicenseInfoResultType> LicenseClient::GetLicenseInfoAsync(
    const RPCParameters& parameters) {
    ::bosdyn::api::GetLicenseInfoRequest request;
    std::promise<GetLicenseInfoResultType> response;
    std::shared_future<GetLicenseInfoResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::GetLicenseInfoRequest, ::bosdyn::api::GetLicenseInfoResponse,
                          ::bosdyn::api::GetLicenseInfoResponse>(
            request,
            std::bind(&::bosdyn::api::LicenseService::Stub::AsyncGetLicenseInfo, m_stub.get(), _1, _2, _3),
            std::bind(&LicenseClient::OnGetLicenseInfoComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

GetLicenseInfoResultType LicenseClient::GetLicenseInfo(const RPCParameters& parameters) {
    return GetLicenseInfoAsync(parameters).get();
}

void LicenseClient::OnGetLicenseInfoComplete(MessagePumpCallBase* call,
                                             const ::bosdyn::api::GetLicenseInfoRequest& request,
                                             ::bosdyn::api::GetLicenseInfoResponse&& response,
                                             const grpc::Status& status,
                                             std::promise<GetLicenseInfoResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::GetLicenseInfoResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<GetFeatureEnabledResultType> LicenseClient::GetFeatureEnabledAsync(
    ::bosdyn::api::GetFeatureEnabledRequest& request, const RPCParameters& parameters) {
    std::promise<GetFeatureEnabledResultType> response;
    std::shared_future<GetFeatureEnabledResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::GetFeatureEnabledRequest, ::bosdyn::api::GetFeatureEnabledResponse,
                          ::bosdyn::api::GetFeatureEnabledResponse>(
            request,
            std::bind(&::bosdyn::api::LicenseService::Stub::AsyncGetFeatureEnabled, m_stub.get(), _1, _2, _3),
            std::bind(&LicenseClient::OnGetFeatureEnabledComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

GetFeatureEnabledResultType LicenseClient::GetFeatureEnabled(::bosdyn::api::GetFeatureEnabledRequest& request,
                                                             const RPCParameters& parameters) {
    return GetFeatureEnabledAsync(request, parameters).get();
}

void LicenseClient::OnGetFeatureEnabledComplete(MessagePumpCallBase* call,
                                                const ::bosdyn::api::GetFeatureEnabledRequest& request,
                                                ::bosdyn::api::GetFeatureEnabledResponse&& response,
                                                const grpc::Status& status,
                                                std::promise<GetFeatureEnabledResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::GetFeatureEnabledResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

// Start of ServiceClient overrides.
ServiceClient::QualityOfService LicenseClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void LicenseClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::LicenseService::Stub(channel));
}
// End of ServiceClient overrides.

}  // namespace client

}  // namespace bosdyn
