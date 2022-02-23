/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/gripper_camera_param/gripper_camera_param_client.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* GripperCameraParamClient::s_default_service_name = "gripper_camera_param";

const char* GripperCameraParamClient::s_service_type = "bosdyn.api.GripperCameraParamService";

GripperCameraSetParamResponseType GripperCameraParamClient::SetGripperCameraParams(
    ::bosdyn::api::GripperCameraParamRequest& request, const RPCParameters& parameters) {
    return SetGripperCameraParamsAsync(request, parameters).get();
}

std::shared_future<GripperCameraSetParamResponseType>
GripperCameraParamClient::SetGripperCameraParamsAsync(
    ::bosdyn::api::GripperCameraParamRequest& request, const RPCParameters& parameters) {
    std::promise<GripperCameraSetParamResponseType> response;
    std::shared_future<GripperCameraSetParamResponseType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");
    MessagePumpCallBase* one_time = InitiateAsyncCall<::bosdyn::api::GripperCameraParamRequest,
                                                      ::bosdyn::api::GripperCameraParamResponse,
                                                      ::bosdyn::api::GripperCameraParamResponse>(
        request,
        std::bind(&::bosdyn::api::GripperCameraParamService::Stub::AsyncSetParams, m_stub.get(), _1,
                  _2, _3),
        std::bind(&GripperCameraParamClient::OnSetGripperCameraParamsComplete, this, _1, _2, _3, _4,
                  _5),
        std::move(response), parameters);

    return future;
}

void GripperCameraParamClient::OnSetGripperCameraParamsComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::GripperCameraParamRequest& request,
    ::bosdyn::api::GripperCameraParamResponse&& response, const grpc::Status& status,
    std::promise<GripperCameraSetParamResponseType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseAndGetFinalStatus<::bosdyn::api::GripperCameraParamResponse>(
            status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

GripperCameraGetParamResponseType GripperCameraParamClient::GetGripperCameraParams(
    const RPCParameters& parameters) {
    return GetGripperCameraParamsAsync(parameters).get();
}

std::shared_future<GripperCameraGetParamResponseType>
GripperCameraParamClient::GetGripperCameraParamsAsync(const RPCParameters& parameters) {
    std::promise<GripperCameraGetParamResponseType> response;
    std::shared_future<GripperCameraGetParamResponseType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::GripperCameraGetParamRequest request;

    MessagePumpCallBase* one_time = InitiateAsyncCall<::bosdyn::api::GripperCameraGetParamRequest,
                                                      ::bosdyn::api::GripperCameraGetParamResponse,
                                                      ::bosdyn::api::GripperCameraGetParamResponse>(
        request,
        std::bind(&::bosdyn::api::GripperCameraParamService::Stub::AsyncGetParams, m_stub.get(), _1,
                  _2, _3),
        std::bind(&GripperCameraParamClient::OnGetGripperCameraParamsComplete, this, _1, _2, _3, _4,
                  _5),
        std::move(response), parameters);

    return future;
}

void GripperCameraParamClient::OnGetGripperCameraParamsComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::GripperCameraGetParamRequest& request,
    ::bosdyn::api::GripperCameraGetParamResponse&& response, const grpc::Status& status,
    std::promise<GripperCameraGetParamResponseType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseAndGetFinalStatus<::bosdyn::api::GripperCameraGetParamResponse>(
            status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

// Start of ServiceClient overrides.
ServiceClient::QualityOfService GripperCameraParamClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void GripperCameraParamClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::GripperCameraParamService::Stub(channel));
}

void GripperCameraParamClient::UpdateServiceFrom(RequestProcessorChain& request_processor_chain,
                                                 ResponseProcessorChain& response_processor_chain,
                                                 const std::shared_ptr<LeaseWallet>& lease_wallet) {
    // Set the lease wallet.
    m_lease_wallet = lease_wallet;

    // Set the request and response processors.
    m_request_processor_chain = request_processor_chain;
    m_response_processor_chain = response_processor_chain;
}
// End of ServiceClient overrides.

}  // namespace client

}  // namespace bosdyn
