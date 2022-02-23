/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/spot_cam/power/power_client.h"
#include "bosdyn/common/success_condition.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

namespace spot_cam {

const char* PowerClient::s_default_service_name = "spot-cam-power";
const char* PowerClient::s_service_type = "bosdyn.api.spot_cam.PowerService";

std::shared_future<GetPowerStatusResultType> PowerClient::GetPowerStatusAsync(
    const RPCParameters& parameters) {
    std::promise<GetPowerStatusResultType> response;
    std::shared_future<GetPowerStatusResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::spot_cam::GetPowerStatusRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::GetPowerStatusRequest,
                          ::bosdyn::api::spot_cam::GetPowerStatusResponse,
                          ::bosdyn::api::spot_cam::GetPowerStatusResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::PowerService::Stub::AsyncGetPowerStatus, m_stub.get(), _1, _2,
                      _3),
            std::bind(&PowerClient::OnGetPowerStatusComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

GetPowerStatusResultType PowerClient::GetPowerStatus(const RPCParameters& parameters) {
    return GetPowerStatusAsync(parameters).get();
}

void PowerClient::OnGetPowerStatusComplete(MessagePumpCallBase* call,
                                           const ::bosdyn::api::spot_cam::GetPowerStatusRequest& request,
                                           ::bosdyn::api::spot_cam::GetPowerStatusResponse&& response,
                                           const grpc::Status& status,
                                           std::promise<GetPowerStatusResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::GetPowerStatusResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<SetPowerStatusResultType> PowerClient::SetPowerStatusAsync(
    bool ptz, bool aux1, bool aux2, bool external_mic, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::SetPowerStatusRequest request;
    request.mutable_status()->mutable_ptz()->set_value(ptz);
    request.mutable_status()->mutable_aux1()->set_value(aux1);
    request.mutable_status()->mutable_aux2()->set_value(aux2);
    request.mutable_status()->mutable_external_mic()->set_value(external_mic);

    return SetPowerStatusAsync(request, parameters);
}

std::shared_future<SetPowerStatusResultType> PowerClient::SetPowerStatusAsync(
    bool ptz, bool aux1, bool aux2, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::SetPowerStatusRequest request;
    request.mutable_status()->mutable_ptz()->set_value(ptz);
    request.mutable_status()->mutable_aux1()->set_value(aux1);
    request.mutable_status()->mutable_aux2()->set_value(aux2);

    return SetPowerStatusAsync(request, parameters);
}

std::shared_future<SetPowerStatusResultType> PowerClient::SetPowerStatusAsync(
    bool ptz, bool aux1, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::SetPowerStatusRequest request;
    request.mutable_status()->mutable_ptz()->set_value(ptz);
    request.mutable_status()->mutable_aux1()->set_value(aux1);

    return SetPowerStatusAsync(request, parameters);
}

std::shared_future<SetPowerStatusResultType> PowerClient::SetPowerStatusAsync(
    bool ptz, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::SetPowerStatusRequest request;
    request.mutable_status()->mutable_ptz()->set_value(ptz);

    return SetPowerStatusAsync(request, parameters);
}

SetPowerStatusResultType PowerClient::SetPowerStatus(bool ptz, bool aux1, bool aux2,
                                                     bool external_mic,
                                                     const RPCParameters& parameters) {
    return SetPowerStatusAsync(ptz, aux1, aux2, external_mic, parameters).get();
}

SetPowerStatusResultType PowerClient::SetPowerStatus(bool ptz, bool aux1, bool aux2,
                                                     const RPCParameters& parameters) {
    return SetPowerStatusAsync(ptz, aux1, aux2, parameters).get();
}

SetPowerStatusResultType PowerClient::SetPowerStatus(bool ptz, bool aux1,
                                                     const RPCParameters& parameters) {
    return SetPowerStatusAsync(ptz, aux1, parameters).get();
}

SetPowerStatusResultType PowerClient::SetPowerStatus(bool ptz, const RPCParameters& parameters) {
    return SetPowerStatusAsync(ptz, parameters).get();
}

std::shared_future<SetPowerStatusResultType> PowerClient::SetPowerStatusAsync(
    ::bosdyn::api::spot_cam::SetPowerStatusRequest& request, const RPCParameters& parameters) {
    std::promise<SetPowerStatusResultType> response;
    std::shared_future<SetPowerStatusResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::SetPowerStatusRequest,
                          ::bosdyn::api::spot_cam::SetPowerStatusResponse,
                          ::bosdyn::api::spot_cam::SetPowerStatusResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::PowerService::Stub::AsyncSetPowerStatus, m_stub.get(), _1, _2,
                      _3),
            std::bind(&PowerClient::OnSetPowerStatusComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

SetPowerStatusResultType PowerClient::SetPowerStatus(::bosdyn::api::spot_cam::SetPowerStatusRequest& request,
                                                     const RPCParameters& parameters) {
    return SetPowerStatusAsync(request, parameters).get();
}

void PowerClient::OnSetPowerStatusComplete(MessagePumpCallBase* call,
                                           const ::bosdyn::api::spot_cam::SetPowerStatusRequest& request,
                                           ::bosdyn::api::spot_cam::SetPowerStatusResponse&& response,
                                           const grpc::Status& status,
                                           std::promise<SetPowerStatusResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::SetPowerStatusResponse>(
        status, response, SDKErrorCode::Success);
    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<CyclePowerResultType> PowerClient::CyclePowerAsync(
    bool ptz, bool aux1, bool aux2, bool external_mic, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::CyclePowerRequest request;
    request.mutable_status()->mutable_ptz()->set_value(ptz);
    request.mutable_status()->mutable_aux1()->set_value(aux1);
    request.mutable_status()->mutable_aux2()->set_value(aux2);
    request.mutable_status()->mutable_external_mic()->set_value(external_mic);

    return CyclePowerAsync(request, parameters);
}

std::shared_future<CyclePowerResultType> PowerClient::CyclePowerAsync(
    bool ptz, bool aux1, bool aux2, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::CyclePowerRequest request;
    request.mutable_status()->mutable_ptz()->set_value(ptz);
    request.mutable_status()->mutable_aux1()->set_value(aux1);
    request.mutable_status()->mutable_aux2()->set_value(aux2);

    return CyclePowerAsync(request, parameters);
}

std::shared_future<CyclePowerResultType> PowerClient::CyclePowerAsync(
    bool ptz, bool aux1, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::CyclePowerRequest request;
    request.mutable_status()->mutable_ptz()->set_value(ptz);
    request.mutable_status()->mutable_aux1()->set_value(aux1);

    return CyclePowerAsync(request, parameters);
}

std::shared_future<CyclePowerResultType> PowerClient::CyclePowerAsync(
    bool ptz, const RPCParameters& parameters) {
    ::bosdyn::api::spot_cam::CyclePowerRequest request;
    request.mutable_status()->mutable_ptz()->set_value(ptz);

    return CyclePowerAsync(request, parameters);
}

CyclePowerResultType PowerClient::CyclePower(bool ptz, bool aux1, bool aux2, bool external_mic,
                                             const RPCParameters& parameters) {
    return CyclePowerAsync(ptz, aux1, aux2, external_mic, parameters).get();
}

CyclePowerResultType PowerClient::CyclePower(bool ptz, bool aux1, bool aux2,
                                             const RPCParameters& parameters) {
    return CyclePowerAsync(ptz, aux1, aux2, parameters).get();
}

CyclePowerResultType PowerClient::CyclePower(bool ptz, bool aux1, const RPCParameters& parameters) {
    return CyclePowerAsync(ptz, aux1, parameters).get();
}

CyclePowerResultType PowerClient::CyclePower(bool ptz, const RPCParameters& parameters) {
    return CyclePowerAsync(ptz, parameters).get();
}

std::shared_future<CyclePowerResultType> PowerClient::CyclePowerAsync(
    ::bosdyn::api::spot_cam::CyclePowerRequest& request, const RPCParameters& parameters) {
    std::promise<CyclePowerResultType> response;
    std::shared_future<CyclePowerResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::spot_cam::CyclePowerRequest, ::bosdyn::api::spot_cam::CyclePowerResponse,
                          ::bosdyn::api::spot_cam::CyclePowerResponse>(
            request,
            std::bind(&::bosdyn::api::spot_cam::PowerService::Stub::AsyncCyclePower, m_stub.get(), _1, _2,
                      _3),
            std::bind(&PowerClient::OnCyclePowerComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

CyclePowerResultType PowerClient::CyclePower(::bosdyn::api::spot_cam::CyclePowerRequest& request,
                                             const RPCParameters& parameters) {
    return CyclePowerAsync(request, parameters).get();
}

void PowerClient::OnCyclePowerComplete(MessagePumpCallBase* call,
                                       const ::bosdyn::api::spot_cam::CyclePowerRequest& request,
                                       ::bosdyn::api::spot_cam::CyclePowerResponse&& response,
                                       const grpc::Status& status,
                                       std::promise<CyclePowerResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::spot_cam::CyclePowerResponse>(
        status, response, SDKErrorCode::Success);
    promise.set_value({ret_status, std::move(response)});
}

ServiceClient::QualityOfService PowerClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void PowerClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::spot_cam::PowerService::Stub(channel));
}

}  // namespace spot_cam
}  // namespace client

}  // namespace bosdyn
