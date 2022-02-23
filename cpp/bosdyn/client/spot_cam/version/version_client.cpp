/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/spot_cam/version/version_client.h"
#include "bosdyn/common/success_condition.h"

using namespace std::placeholders;

namespace {

auto constexpr kGenericStubError = "Stub is not set in Version Client";

}

namespace bosdyn {

namespace client {

namespace spot_cam {

const char* VersionClient::s_default_service_name = "spot-cam-version";
const char* VersionClient::s_service_type = "bosdyn.api.spot_cam.VersionService";

std::shared_future<GetSoftwareVersionResultType> VersionClient::GetSoftwareVersionAsync(
    const RPCParameters& parameters) {
    std::promise<GetSoftwareVersionResultType> response;
    std::shared_future<GetSoftwareVersionResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::spot_cam::GetSoftwareVersionRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::GetSoftwareVersionRequest, ::bosdyn::api::spot_cam::GetSoftwareVersionResponse,
                          ::bosdyn::api::spot_cam::GetSoftwareVersionResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::VersionService::Stub::AsyncGetSoftwareVersion, m_stub.get(), _1, _2, _3),
            std::bind(&VersionClient::OnGetSoftwareVersionComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

GetSoftwareVersionResultType VersionClient::GetSoftwareVersion(
    const RPCParameters& parameters) {
    return GetSoftwareVersionAsync(parameters).get();
}

void VersionClient::OnGetSoftwareVersionComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::spot_cam::GetSoftwareVersionRequest& request,
    ::bosdyn::api::spot_cam::GetSoftwareVersionResponse&& response, const grpc::Status& status,
    std::promise<GetSoftwareVersionResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::GetSoftwareVersionResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

ServiceClient::QualityOfService VersionClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void VersionClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::spot_cam::VersionService::Stub(channel));
}

}  // namespace spot_cam
}  // namespace client

}  // namespace bosdyn
