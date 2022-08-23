/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/mission/mission_service.grpc.pb.h>
#include <bosdyn/api/mission/mission_service.pb.h>
#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/client/lease/lease_resources.h"
#include "bosdyn/client/mission/mission_error_codes.h"

namespace bosdyn {

namespace client {
namespace mission {
typedef Result<::bosdyn::api::mission::LoadMissionResponse> LoadMissionResultType;
typedef Result<::bosdyn::api::mission::PauseMissionResponse> PauseMissionResultType;
typedef Result<::bosdyn::api::mission::PlayMissionResponse> PlayMissionResultType;
typedef Result<::bosdyn::api::mission::RestartMissionResponse> RestartMissionResultType;
typedef Result<::bosdyn::api::mission::GetStateResponse> GetStateResultType;
typedef Result<::bosdyn::api::mission::GetInfoResponse> GetInfoResultType;
typedef Result<::bosdyn::api::mission::GetMissionResponse> GetMissionResultType;
typedef Result<::bosdyn::api::mission::AnswerQuestionResponse> AnswerQuestionResultType;

class MissionClient : public ServiceClient {
 public:
    MissionClient() = default;

    ~MissionClient() = default;

    // Uploads a mission to the robot which can be run later.
    // Note: the LoadMission RPC requires a lease. By default, we use a body lease, however other
    // resources (or no resources) can be provided to change which leases are attached to the
    // request. If the request is larger than 100mb, LoadMissionAsChunks should be used.
    LoadMissionResultType LoadMission(
        ::bosdyn::api::mission::LoadMissionRequest& request,
        const ::bosdyn::client::RPCParameters& parameters = ::bosdyn::client::RPCParameters(),
        const std::vector<std::string>& desired_lease_resources = {::bosdyn::client::kBodyResource});
    std::shared_future<LoadMissionResultType> LoadMissionAsync(
        ::bosdyn::api::mission::LoadMissionRequest& request,
        const RPCParameters& parameters = RPCParameters(),
        const std::vector<std::string>& desired_lease_resources = {::bosdyn::client::kBodyResource});

    // Uploads a mission to the robot which can be run later.
    // Note: the LoadMission RPC requires a lease. By default, we use a body lease, however other
    // resources (or no resources) can be provided to change which leases are attached to the
    // request.
    LoadMissionResultType LoadMissionAsChunks(
        ::bosdyn::api::mission::LoadMissionRequest& request,
        const ::bosdyn::client::RPCParameters& parameters = ::bosdyn::client::RPCParameters(),
        const std::vector<std::string>& desired_lease_resources = {::bosdyn::client::kBodyResource});
    std::shared_future<LoadMissionResultType> LoadMissionAsChunksAsync(
        ::bosdyn::api::mission::LoadMissionRequest& request,
        const RPCParameters& parameters = RPCParameters(),
        const std::vector<std::string>& desired_lease_resources = {::bosdyn::client::kBodyResource});

    // Pause the execution of a mission until further notified (via PlayMission).
    // Note: the PauseMission RPC requires a lease. By default, we use a body lease, however other
    // resources (or no resources) can be provided to change which leases are attached to the request.
    PauseMissionResultType PauseMission(
        ::bosdyn::api::mission::PauseMissionRequest& request,
        const ::bosdyn::client::RPCParameters& parameters = ::bosdyn::client::RPCParameters());
    std::shared_future<PauseMissionResultType> PauseMissionAsync(
        ::bosdyn::api::mission::PauseMissionRequest& request,
        const RPCParameters& parameters = RPCParameters(),
        const std::string& desired_lease_resource = ::bosdyn::client::kBodyResource);

    // Start executing a loaded mission. This will not restart a mission that has completed fully - to
    // do that, use RestartMission.
    // Note: the PlayMission RPC requires a lease. By default, we use a body lease, however other
    // resources (or no resources) can be provided to change which leases are attached to the request.
    PlayMissionResultType PlayMission(
        ::bosdyn::api::mission::PlayMissionRequest& request,
        const ::bosdyn::client::RPCParameters& parameters = ::bosdyn::client::RPCParameters());
    std::shared_future<PlayMissionResultType> PlayMissionAsync(
        ::bosdyn::api::mission::PlayMissionRequest& request,
        const RPCParameters& parameters = RPCParameters(),
        const std::vector<std::string>& desired_lease_resources = {::bosdyn::client::kBodyResource});

    // Start executing a loaded mission from the beginning.
    // Note: the RestartMission RPC requires a lease. By default, we use a body lease, however other
    // resources (or no resources) can be provided to change which leases are attached to the request.
    RestartMissionResultType RestartMission(
        ::bosdyn::api::mission::RestartMissionRequest& request,
        const ::bosdyn::client::RPCParameters& parameters = ::bosdyn::client::RPCParameters());
    std::shared_future<RestartMissionResultType> RestartMissionAsync(
        ::bosdyn::api::mission::RestartMissionRequest& request,
        const RPCParameters& parameters = RPCParameters(),
        const std::vector<std::string>& desired_lease_resources = {::bosdyn::client::kBodyResource});

    GetStateResultType GetState(
        ::bosdyn::api::mission::GetStateRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetStateResultType> GetStateAsync(
        ::bosdyn::api::mission::GetStateRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetStateResultType> GetStateAsync(
        const RPCParameters& parameters = RPCParameters());

    GetInfoResultType GetInfo(const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetInfoResultType> GetInfoAsync(
        const RPCParameters& parameters = RPCParameters());

    GetMissionResultType GetMission(
        ::bosdyn::api::mission::GetMissionRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<GetMissionResultType> GetMissionAsync(
        ::bosdyn::api::mission::GetMissionRequest& request,
        const RPCParameters& parameters = RPCParameters());

    AnswerQuestionResultType AnswerQuestion(
        ::bosdyn::api::mission::AnswerQuestionRequest& request,
        const RPCParameters& parameters = RPCParameters());

    std::shared_future<AnswerQuestionResultType> AnswerQuestionAsync(
        ::bosdyn::api::mission::AnswerQuestionRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    // Override the update from method to set the lease wallet for the robot command client
    // to be the same as the lease wallet of the robot object.
    void UpdateServiceFrom(
        RequestProcessorChain& request_processor_chain,
        ResponseProcessorChain& response_processor_chain,
        const std::shared_ptr<LeaseWallet>& lease_wallet) override;
    // End of ServiceClient overrides.

    // Get the default service name the mission service will be registered in the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the mission service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous grpc calls.
    void OnLoadMissionComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::mission::LoadMissionRequest& request,
        ::bosdyn::api::mission::LoadMissionResponse&& response, const grpc::Status& status,
        std::promise<LoadMissionResultType> promise);

    void OnLoadMissionAsChunksComplete(
        MessagePumpCallBase* call, const std::vector<::bosdyn::api::DataChunk>&& request,
        ::bosdyn::api::mission::LoadMissionResponse&& response, const grpc::Status& status,
        std::promise<LoadMissionResultType> promise);

    void OnPauseMissionComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::mission::PauseMissionRequest& request,
        ::bosdyn::api::mission::PauseMissionResponse&& response, const grpc::Status& status,
        std::promise<PauseMissionResultType> promise);

    void OnPlayMissionComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::mission::PlayMissionRequest& request,
        ::bosdyn::api::mission::PlayMissionResponse&& response, const grpc::Status& status,
        std::promise<PlayMissionResultType> promise);

    void OnRestartMissionComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::mission::RestartMissionRequest& request,
        ::bosdyn::api::mission::RestartMissionResponse&& response, const grpc::Status& status,
        std::promise<RestartMissionResultType> promise);

    void OnGetStateComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::mission::GetStateRequest& request,
        ::bosdyn::api::mission::GetStateResponse&& response, const grpc::Status& status,
        std::promise<GetStateResultType> promise);

    void OnGetInfoComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::mission::GetInfoRequest& request,
        ::bosdyn::api::mission::GetInfoResponse&& response, const grpc::Status& status,
        std::promise<GetInfoResultType> promise);

    void OnGetMissionComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::mission::GetMissionRequest& request,
        ::bosdyn::api::mission::GetMissionResponse&& response, const grpc::Status& status,
        std::promise<GetMissionResultType> promise);

    void OnAnswerQuestionComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::mission::AnswerQuestionRequest& request,
        ::bosdyn::api::mission::AnswerQuestionResponse&& response, const grpc::Status& status,
        std::promise<AnswerQuestionResultType> promise);

    std::unique_ptr<::bosdyn::api::mission::MissionService::Stub> m_stub;

    // Default service name for the mission service.
    static const char* s_default_service_name;

    // Default service type for the mission service.
    static const char* s_service_type;

    // Lease wallet for commands.
    std::shared_ptr<LeaseWallet> m_lease_wallet;
};
}  // namespace mission
}  // namespace client

}  // namespace bosdyn
