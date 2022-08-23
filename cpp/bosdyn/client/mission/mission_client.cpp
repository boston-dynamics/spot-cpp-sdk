/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/lease/lease_processors.h"
#include "bosdyn/client/mission/mission_client.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

namespace mission {
const char* MissionClient::s_default_service_name = "robot-mission";

const char* MissionClient::s_service_type = "bosdyn.api.mission.MissionService";

std::shared_future<LoadMissionResultType> MissionClient::LoadMissionAsync(
    ::bosdyn::api::mission::LoadMissionRequest& request,
    const ::bosdyn::client::RPCParameters& parameters, const std::vector<std::string>& desired_lease_resources) {
    std::promise<LoadMissionResultType> response;
    std::shared_future<LoadMissionResultType> future = response.get_future();
    // Run a lease processor function to attempt to automatically apply a lease to the request if
    // a lease is not already set.
    auto lease_status = ProcessRequestWithMultipleLeases(&request, m_lease_wallet.get(),
                                                         desired_lease_resources);
    if (!lease_status) {
        // Failed to set a lease with the lease wallet. Return early since the request will fail
        // without a lease.
        response.set_value({lease_status, {}});
        return future;
    }

    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::mission::LoadMissionRequest, ::bosdyn::api::mission::LoadMissionResponse,
                          ::bosdyn::api::mission::LoadMissionResponse>(
            request,
            std::bind(&::bosdyn::api::mission::MissionService::Stub::AsyncLoadMission, m_stub.get(), _1, _2,
                      _3),
            std::bind(&MissionClient::OnLoadMissionComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

void MissionClient::OnLoadMissionComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::mission::LoadMissionRequest& request,
    ::bosdyn::api::mission::LoadMissionResponse&& response, const grpc::Status& status,
    std::promise<LoadMissionResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseWithMultiLeaseAndGetFinalStatus<::bosdyn::api::mission::LoadMissionResponse>(
            status, response, response.status(), m_lease_wallet.get());
    promise.set_value({ret_status, std::move(response)});
}

LoadMissionResultType MissionClient::LoadMission(
    ::bosdyn::api::mission::LoadMissionRequest& request, const ::bosdyn::client::RPCParameters& parameters,
    const std::vector<std::string>& desired_lease_resources){
    return LoadMissionAsync(request, parameters, desired_lease_resources).get();
}

LoadMissionResultType MissionClient::LoadMissionAsChunks(
    ::bosdyn::api::mission::LoadMissionRequest& request,
    const ::bosdyn::client::RPCParameters& parameters,
    const std::vector<std::string>& desired_lease_resources) {
    return LoadMissionAsChunksAsync(request, parameters, desired_lease_resources).get();       
}

std::shared_future<LoadMissionResultType> MissionClient::LoadMissionAsChunksAsync(
    ::bosdyn::api::mission::LoadMissionRequest& request,
    const RPCParameters& parameters,
    const std::vector<std::string>& desired_lease_resources) {
    std::promise<LoadMissionResultType> response;
    std::shared_future<LoadMissionResultType> future = response.get_future();

    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    // Run a lease processor function to attempt to automatically apply a lease to the request if
    // a lease is not already set.
    auto lease_status = ProcessRequestWithMultipleLeases(&request, m_lease_wallet.get(),
                                                         desired_lease_resources);

    if (!lease_status) {
        // Failed to set a lease with the lease wallet. Return early since the request will fail
        // without a lease.
        response.set_value({lease_status, {}});
        return future;
    }

    MessagePumpCallBase* one_time =
        InitiateRequestStreamAsyncCallWithChunking<::bosdyn::api::mission::LoadMissionRequest,
                                                   ::bosdyn::api::mission::LoadMissionResponse,
                                                   ::bosdyn::api::mission::LoadMissionResponse>(
        std::move(request),
        std::bind(&::bosdyn::api::mission::MissionService::Stub::AsyncLoadMissionAsChunks, m_stub.get(),
                  _1, _2, _3, _4),
        std::bind(&MissionClient::OnLoadMissionAsChunksComplete, this, _1, _2, _3, _4, _5),
        std::move(response), parameters);

    return future;
}

void MissionClient::OnLoadMissionAsChunksComplete(
    MessagePumpCallBase* call, const std::vector<::bosdyn::api::DataChunk>&& request,
    ::bosdyn::api::mission::LoadMissionResponse&& response, const grpc::Status& status,
    std::promise<LoadMissionResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseWithMultiLeaseAndGetFinalStatus<::bosdyn::api::mission::LoadMissionResponse>(
            status, response, response.status(), m_lease_wallet.get());
    promise.set_value({ret_status, std::move(response)});
}

std::shared_future<PauseMissionResultType> MissionClient::PauseMissionAsync(
    ::bosdyn::api::mission::PauseMissionRequest& request,
    const ::bosdyn::client::RPCParameters& parameters, const std::string& desired_lease_resource) {
    std::promise<PauseMissionResultType> response;
    std::shared_future<PauseMissionResultType> future = response.get_future();
    // Run a lease processor function to attempt to automatically apply a lease to the request if
    // a lease is not already set.
    auto lease_status =
        ProcessRequestWithLease(&request, m_lease_wallet.get(), desired_lease_resource);
    if (!lease_status) {
        // Failed to set a lease with the lease wallet. Return early since the request will fail
        // without a lease.
        response.set_value({lease_status, {}});
        return future;
    }

    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::mission::PauseMissionRequest, ::bosdyn::api::mission::PauseMissionResponse,
                          ::bosdyn::api::mission::PauseMissionResponse>(
            request,
            std::bind(&::bosdyn::api::mission::MissionService::Stub::AsyncPauseMission, m_stub.get(), _1, _2,
                      _3),
            std::bind(&MissionClient::OnPauseMissionComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

void MissionClient::OnPauseMissionComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::mission::PauseMissionRequest& request,
    ::bosdyn::api::mission::PauseMissionResponse&& response, const grpc::Status& status,
    std::promise<PauseMissionResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseWithLeaseAndGetFinalStatus<::bosdyn::api::mission::PauseMissionResponse>(
            status, response, response.status(), m_lease_wallet.get());
    promise.set_value({ret_status, std::move(response)});
}

PauseMissionResultType MissionClient::PauseMission(::bosdyn::api::mission::PauseMissionRequest& request,
                                                   const ::bosdyn::client::RPCParameters& parameters) {
    return PauseMissionAsync(request, parameters).get();
}

std::shared_future<PlayMissionResultType> MissionClient::PlayMissionAsync(
    ::bosdyn::api::mission::PlayMissionRequest& request,
    const ::bosdyn::client::RPCParameters& parameters,
    const std::vector<std::string>& desired_lease_resources) {
    std::promise<PlayMissionResultType> response;
    std::shared_future<PlayMissionResultType> future = response.get_future();

    // Run a lease processor function to attempt to automatically apply a lease to the request if
    // a lease is not already set.
    auto lease_status = ProcessRequestWithMultipleLeases(&request, m_lease_wallet.get(),
                                                         desired_lease_resources);
    if (!lease_status) {
        // Failed to set a lease with the lease wallet. Return early since the request will fail
        // without a lease.
        response.set_value({lease_status, {}});
        return future;
    }

    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::mission::PlayMissionRequest, ::bosdyn::api::mission::PlayMissionResponse,
                          ::bosdyn::api::mission::PlayMissionResponse>(
            request,
            std::bind(&::bosdyn::api::mission::MissionService::Stub::AsyncPlayMission, m_stub.get(), _1, _2,
                      _3),
            std::bind(&MissionClient::OnPlayMissionComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

void MissionClient::OnPlayMissionComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::mission::PlayMissionRequest& request,
    ::bosdyn::api::mission::PlayMissionResponse&& response, const grpc::Status& status,
    std::promise<PlayMissionResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseWithMultiLeaseAndGetFinalStatus<::bosdyn::api::mission::PlayMissionResponse>(
            status, response, response.status(), m_lease_wallet.get());
    promise.set_value({ret_status, std::move(response)});
}

PlayMissionResultType MissionClient::PlayMission(::bosdyn::api::mission::PlayMissionRequest& request,
                                                 const ::bosdyn::client::RPCParameters& parameters) {
    return PlayMissionAsync(request, parameters).get();
}

std::shared_future<RestartMissionResultType> MissionClient::RestartMissionAsync(
    ::bosdyn::api::mission::RestartMissionRequest& request,
    const ::bosdyn::client::RPCParameters& parameters,
    const std::vector<std::string>& desired_lease_resources) {
    std::promise<RestartMissionResultType> response;
    std::shared_future<RestartMissionResultType> future = response.get_future();

    // Run a lease processor function to attempt to automatically apply a lease to the request if
    // a lease is not already set.
    auto lease_status = ProcessRequestWithMultipleLeases(&request, m_lease_wallet.get(),
                                                         desired_lease_resources);
    if (!lease_status) {
        // Failed to set a lease with the lease wallet. Return early since the request will fail
        // without a lease.
        response.set_value({lease_status, {}});
        return future;
    }

    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::mission::RestartMissionRequest, ::bosdyn::api::mission::RestartMissionResponse,
                          ::bosdyn::api::mission::RestartMissionResponse>(
            request,
            std::bind(&::bosdyn::api::mission::MissionService::Stub::AsyncRestartMission, m_stub.get(), _1,
                      _2, _3),
            std::bind(&MissionClient::OnRestartMissionComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

void MissionClient::OnRestartMissionComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::mission::RestartMissionRequest& request,
    ::bosdyn::api::mission::RestartMissionResponse&& response, const grpc::Status& status,
    std::promise<RestartMissionResultType> promise) {
    ::bosdyn::common::Status ret_status =
        ProcessResponseWithMultiLeaseAndGetFinalStatus<::bosdyn::api::mission::RestartMissionResponse>(
            status, response, response.status(), m_lease_wallet.get());
    promise.set_value({ret_status, std::move(response)});
}

RestartMissionResultType MissionClient::RestartMission(::bosdyn::api::mission::RestartMissionRequest& request,
                                                       const ::bosdyn::client::RPCParameters& parameters) {
    return RestartMissionAsync(request, parameters).get();
}

std::shared_future<GetStateResultType> MissionClient::GetStateAsync(
    ::bosdyn::api::mission::GetStateRequest& request,
    const ::bosdyn::client::RPCParameters& parameters) {
    std::promise<GetStateResultType> response;
    std::shared_future<GetStateResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::mission::GetStateRequest, ::bosdyn::api::mission::GetStateResponse,
                          ::bosdyn::api::mission::GetStateResponse>(
            request,
            std::bind(&::bosdyn::api::mission::MissionService::Stub::AsyncGetState, m_stub.get(), _1, _2, _3),
            std::bind(&MissionClient::OnGetStateComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

/**
 * Get the state of the current mission, passing an empty GetStateRequest proto.
 */
std::shared_future<GetStateResultType> MissionClient::GetStateAsync(
    const ::bosdyn::client::RPCParameters& parameters) {
    ::bosdyn::api::mission::GetStateRequest request;
    return GetStateAsync(request, parameters);
}

void MissionClient::OnGetStateComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::mission::GetStateRequest& request,
    ::bosdyn::api::mission::GetStateResponse&& response,const grpc::Status& status,
    std::promise<GetStateResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::mission::GetStateResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

GetStateResultType MissionClient::GetState(::bosdyn::api::mission::GetStateRequest& request,
                                           const ::bosdyn::client::RPCParameters& parameters) {
    return GetStateAsync(request, parameters).get();
}

std::shared_future<GetInfoResultType> MissionClient::GetInfoAsync(
    const ::bosdyn::client::RPCParameters& parameters) {
    std::promise<GetInfoResultType> response;
    std::shared_future<GetInfoResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");
    ::bosdyn::api::mission::GetInfoRequest request;
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::mission::GetInfoRequest, ::bosdyn::api::mission::GetInfoResponse,
                          ::bosdyn::api::mission::GetInfoResponse>(
            request,
            std::bind(&::bosdyn::api::mission::MissionService::Stub::AsyncGetInfo, m_stub.get(), _1, _2, _3),
            std::bind(&MissionClient::OnGetInfoComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

void MissionClient::OnGetInfoComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::mission::GetInfoRequest& request,
    ::bosdyn::api::mission::GetInfoResponse&& response, const grpc::Status& status,
    std::promise<GetInfoResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::mission::GetInfoResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

GetInfoResultType MissionClient::GetInfo(const ::bosdyn::client::RPCParameters& parameters) {
    return GetInfoAsync(parameters).get();
}

std::shared_future<GetMissionResultType> MissionClient::GetMissionAsync(
    ::bosdyn::api::mission::GetMissionRequest& request,
    const ::bosdyn::client::RPCParameters& parameters) {
    std::promise<GetMissionResultType> response;
    std::shared_future<GetMissionResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::mission::GetMissionRequest, ::bosdyn::api::mission::GetMissionResponse,
                          ::bosdyn::api::mission::GetMissionResponse>(
            request,
            std::bind(&::bosdyn::api::mission::MissionService::Stub::AsyncGetMission, m_stub.get(), _1, _2,
                      _3),
            std::bind(&MissionClient::OnGetMissionComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

void MissionClient::OnGetMissionComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::mission::GetMissionRequest& request,
    ::bosdyn::api::mission::GetMissionResponse&& response, const grpc::Status& status,
    std::promise<GetMissionResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::mission::GetMissionResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

GetMissionResultType MissionClient::GetMission(::bosdyn::api::mission::GetMissionRequest& request,
                                               const ::bosdyn::client::RPCParameters& parameters) {
    return GetMissionAsync(request, parameters).get();
}

std::shared_future<AnswerQuestionResultType> MissionClient::AnswerQuestionAsync(
    ::bosdyn::api::mission::AnswerQuestionRequest& request,
    const ::bosdyn::client::RPCParameters& parameters) {
    std::promise<AnswerQuestionResultType> response;
    std::shared_future<AnswerQuestionResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::mission::AnswerQuestionRequest, ::bosdyn::api::mission::AnswerQuestionResponse,
                          ::bosdyn::api::mission::AnswerQuestionResponse>(
            request,
            std::bind(&::bosdyn::api::mission::MissionService::Stub::AsyncAnswerQuestion, m_stub.get(), _1,
                      _2, _3),
            std::bind(&MissionClient::OnAnswerQuestionComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

void MissionClient::OnAnswerQuestionComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::mission::AnswerQuestionRequest& request,
    ::bosdyn::api::mission::AnswerQuestionResponse&& response, const grpc::Status& status,
    std::promise<AnswerQuestionResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::mission::AnswerQuestionResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

AnswerQuestionResultType MissionClient::AnswerQuestion(::bosdyn::api::mission::AnswerQuestionRequest& request,
                                                       const ::bosdyn::client::RPCParameters& parameters) {
    return AnswerQuestionAsync(request, parameters).get();
}

ServiceClient::QualityOfService MissionClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void MissionClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::mission::MissionService::Stub(channel));
}

void MissionClient::UpdateServiceFrom(
    RequestProcessorChain& request_processor_chain,
    ResponseProcessorChain& response_processor_chain,
    const std::shared_ptr<LeaseWallet>& lease_wallet) {
    // Set the lease wallet.
    m_lease_wallet = lease_wallet;

    // Set the request and response processors.
    m_request_processor_chain = request_processor_chain;
    m_response_processor_chain = response_processor_chain;
}

}  // namespace mission
}  // namespace client

}  // namespace bosdyn
