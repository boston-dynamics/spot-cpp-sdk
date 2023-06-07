/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "manipulation_api_client.h"
#include "bosdyn/client/lease/lease_resources.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* ManipulationApiClient::s_default_service_name = "manipulation";

const char* ManipulationApiClient::s_service_type = "bosdyn.api.ManipulationApiService";

std::shared_future<ManipulationApiResultType> ManipulationApiClient::ManipulationApiAsync(
    ::bosdyn::api::ManipulationApiRequest& request, const RPCParameters& parameters) {
    std::promise<ManipulationApiResultType> response;
    std::shared_future<ManipulationApiResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");
    // Run a lease processor function to attempt to automatically apply a lease to the request if
    // a lease is not already set.
    auto lease_status = ::bosdyn::client::ProcessRequestWithLease(&request, m_lease_wallet.get(),
                                                                  ::bosdyn::client::kBodyResource);
    if (!lease_status) {
        // Failed to set a lease with the lease wallet. Return early since the request will fail
        // without a lease.
        response.set_value({lease_status, {}});
        return future;
    }

    MessagePumpCallBase* one_time = InitiateAsyncCall<::bosdyn::api::ManipulationApiRequest,
                                                      ::bosdyn::api::ManipulationApiResponse,
                                                      ::bosdyn::api::ManipulationApiResponse>(
        request,
        std::bind(&::bosdyn::api::ManipulationApiService::Stub::AsyncManipulationApi, m_stub.get(),
                  _1, _2, _3),
        std::bind(&ManipulationApiClient::OnManipulationApiComplete, this, _1, _2, _3, _4, _5),
        std::move(response), parameters);

    return future;
}

void ManipulationApiClient::OnManipulationApiComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::ManipulationApiRequest& request,
    ::bosdyn::api::ManipulationApiResponse&& response, const grpc::Status& status,
    std::promise<ManipulationApiResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseWithLeaseAndGetFinalStatus<::bosdyn::api::ManipulationApiResponse>(
            status, response, SDKErrorCode::Success, m_lease_wallet.get());
    promise.set_value({ret_status, std::move(response)});
}

ManipulationApiResultType ManipulationApiClient::ManipulationApi(
    ::bosdyn::api::ManipulationApiRequest& request, const RPCParameters& parameters) {
    return ManipulationApiAsync(request, parameters).get();
}

std::shared_future<ManipulationApiFeedbackResultType>
ManipulationApiClient::ManipulationApiFeedbackAsync(
    ::bosdyn::api::ManipulationApiFeedbackRequest& request, const RPCParameters& parameters) {
    std::promise<ManipulationApiFeedbackResultType> response;
    std::shared_future<ManipulationApiFeedbackResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::ManipulationApiFeedbackRequest,
                          ::bosdyn::api::ManipulationApiFeedbackResponse,
                          ::bosdyn::api::ManipulationApiFeedbackResponse>(
            request,
            std::bind(&::bosdyn::api::ManipulationApiService::Stub::AsyncManipulationApiFeedback,
                      m_stub.get(), _1, _2, _3),
            std::bind(&ManipulationApiClient::OnManipulationApiFeedbackComplete, this, _1, _2, _3,
                      _4, _5),
            std::move(response), parameters);

    return future;
}

void ManipulationApiClient::OnManipulationApiFeedbackComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::ManipulationApiFeedbackRequest& request,
    ::bosdyn::api::ManipulationApiFeedbackResponse&& response, const grpc::Status& status,
    std::promise<ManipulationApiFeedbackResultType> promise) {
    // Feedback indicates the command conditions and does not represent an error code. As
    // such, the response.status will always be a success, regardless of the feedback status value.
    ::bosdyn::common::Status ret_status =
        ProcessResponseAndGetFinalStatus<::bosdyn::api::ManipulationApiFeedbackResponse>(
            status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

ManipulationApiFeedbackResultType ManipulationApiClient::ManipulationApiFeedback(
    ::bosdyn::api::ManipulationApiFeedbackRequest& request, const RPCParameters& parameters) {
    return ManipulationApiFeedbackAsync(request, parameters).get();
}

std::shared_future<OverrideGraspResultType> ManipulationApiClient::OverrideGraspAsync(
    ::bosdyn::api::ApiGraspOverrideRequest& request, const RPCParameters& parameters) {
    std::promise<OverrideGraspResultType> response;
    std::shared_future<OverrideGraspResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time = InitiateAsyncCall<::bosdyn::api::ApiGraspOverrideRequest,
                                                      ::bosdyn::api::ApiGraspOverrideResponse,
                                                      ::bosdyn::api::ApiGraspOverrideResponse>(
        request,
        std::bind(&::bosdyn::api::ManipulationApiService::Stub::AsyncOverrideGrasp, m_stub.get(),
                  _1, _2, _3),
        std::bind(&ManipulationApiClient::OnOverrideGraspComplete, this, _1, _2, _3, _4, _5),
        std::move(response), parameters);

    return future;
}

void ManipulationApiClient::OnOverrideGraspComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::ApiGraspOverrideRequest& request,
    ::bosdyn::api::ApiGraspOverrideResponse&& response, const grpc::Status& status,
    std::promise<OverrideGraspResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseAndGetFinalStatus<::bosdyn::api::ApiGraspOverrideResponse>(
            status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

OverrideGraspResultType ManipulationApiClient::OverrideGrasp(
    ::bosdyn::api::ApiGraspOverrideRequest& request, const RPCParameters& parameters) {
    return OverrideGraspAsync(request, parameters).get();
}

ServiceClient::QualityOfService ManipulationApiClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void ManipulationApiClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::ManipulationApiService::Stub(channel));
}

void ManipulationApiClient::UpdateServiceFrom(RequestProcessorChain& request_processor_chain,
                                              ResponseProcessorChain& response_processor_chain,
                                              const std::shared_ptr<LeaseWallet>& lease_wallet) {
    // Set the lease wallet.
    m_lease_wallet = lease_wallet;

    // Set the request and response processors.
    m_request_processor_chain = request_processor_chain;
    m_response_processor_chain = response_processor_chain;
}

}  // namespace client

}  // namespace bosdyn
