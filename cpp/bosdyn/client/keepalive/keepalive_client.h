/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <future>

#include <bosdyn/api/keepalive/keepalive_service.grpc.pb.h>
#include <bosdyn/api/keepalive/keepalive_service.pb.h>

#include "bosdyn/client/keepalive/keepalive_error_codes.h"
#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/common/status.h"

namespace bosdyn {

namespace client {

typedef Result<::bosdyn::api::keepalive::ModifyPolicyResponse> ModifyPolicyResultType;
typedef Result<::bosdyn::api::keepalive::CheckInResponse> CheckInResultType;
typedef Result<::bosdyn::api::keepalive::GetStatusResponse> GetStatusResultType;

// KeepaliveClient is the GRPC client for the Auto Return service defined in
// keepalive_service.proto.
// This client is in BETA and may undergo changes in future releases.
class KeepaliveClient : public ServiceClient {
 public:
    KeepaliveClient() = default;

    ~KeepaliveClient() = default;

    // Asynchronous method to add or remove policies.
    std::shared_future<ModifyPolicyResultType> ModifyPolicyAsync(
        ::bosdyn::api::keepalive::ModifyPolicyRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to add or remove policies.
    ModifyPolicyResultType ModifyPolicy(::bosdyn::api::keepalive::ModifyPolicyRequest& request,
                                        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to get the keepalive system status.
    std::shared_future<GetStatusResultType> GetStatusAsync(
        ::bosdyn::api::keepalive::GetStatusRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to get the keepalive system status.
    GetStatusResultType GetStatus(::bosdyn::api::keepalive::GetStatusRequest& request,
                                  const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to check into a policy.
    std::shared_future<CheckInResultType> CheckInAsync(
        ::bosdyn::api::keepalive::CheckInRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to check into a policy.
    CheckInResultType CheckIn(::bosdyn::api::keepalive::CheckInRequest& request,
                              const RPCParameters& parameters = RPCParameters());

    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;

    // Override the update from method to set the lease wallet for the robot command client
    // to be the same as the lease wallet of the robot object.
    void UpdateServiceFrom(RequestProcessorChain& request_processor_chain,
                           ResponseProcessorChain& response_processor_chain,
                           const std::shared_ptr<LeaseWallet>& lease_wallet) override;
    // End of ServiceClient overrides.

    // Get the default service name the Keepaliveservice will be registered in the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the default service type for the Keepalive service that will be registered in the
    // directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback that will return the ModifyPolicyResponse message after ModifyPolicy rpc
    // returns to the client.
    void OnModifyPolicyComplete(MessagePumpCallBase* call,
                                const ::bosdyn::api::keepalive::ModifyPolicyRequest& request,
                                ::bosdyn::api::keepalive::ModifyPolicyResponse&& response,
                                const grpc::Status& status,
                                std::promise<ModifyPolicyResultType> promise);

    // Callback that will return the GetStatusResponse message after GetStatus rpc
    // returns to the client.
    void OnGetStatusComplete(MessagePumpCallBase* call,
                             const ::bosdyn::api::keepalive::GetStatusRequest& request,
                             ::bosdyn::api::keepalive::GetStatusResponse&& response,
                             const grpc::Status& status, std::promise<GetStatusResultType> promise);

    // Callback that will return the CheckInResponse message after CheckIn rpc returns
    // to the client.
    void OnCheckInComplete(MessagePumpCallBase* call,
                           const ::bosdyn::api::keepalive::CheckInRequest& request,
                           ::bosdyn::api::keepalive::CheckInResponse&& response,
                           const grpc::Status& status, std::promise<CheckInResultType> promise);

    std::unique_ptr<::bosdyn::api::keepalive::KeepaliveService::Stub> m_stub;

    // Default service name for the Auto Return service.
    static const char* s_default_service_name;

    // Default service type for the Keepalive service.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
