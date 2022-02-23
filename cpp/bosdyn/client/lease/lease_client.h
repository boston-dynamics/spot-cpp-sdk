/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/lease.pb.h>
#include <bosdyn/api/lease_service.grpc.pb.h>
#include <bosdyn/api/lease_service.pb.h>

#include <future>

#include "lease.h"
#include "bosdyn/client/service_client/service_client.h"
#include "bosdyn/common/status.h"
#include "bosdyn/client/lease/lease_resources.h"

namespace bosdyn {

namespace client {

typedef Result<::bosdyn::api::AcquireLeaseResponse> AcquireLeaseResultType;
typedef Result<::bosdyn::api::TakeLeaseResponse> TakeLeaseResultType;
typedef Result<::bosdyn::api::ReturnLeaseResponse> ReturnLeaseResultType;
typedef Result<::bosdyn::api::ListLeasesResponse> ListLeasesResultType;
typedef Result<::bosdyn::api::RetainLeaseResponse> RetainLeaseResultType;

// LeaseClient maintains the liveness of Leases that it manages.
//
// Implementations must be written in a thread-safe manner.
class LeaseClient : public ServiceClient {
 public:
    LeaseClient() = default;
    ~LeaseClient() = default;

    // Asynchronous method to acquire a lease for a resource, if resource available.
    std::shared_future<AcquireLeaseResultType> AcquireLeaseAsync(
        const std::string& resource,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to issue an acquire lease request.
    AcquireLeaseResultType AcquireLease(
        const std::string& resource,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to take a lease for a resource, even if another client has a lease.
    std::shared_future<TakeLeaseResultType> TakeLeaseAsync(
        const std::string& resource,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to issue a take lease request.
    TakeLeaseResultType TakeLease(
        const std::string& resource,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to return a lease for a resource.
    std::shared_future<ReturnLeaseResultType> ReturnLeaseAsync(
        ::bosdyn::api::ReturnLeaseRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to issue a return lease request.
    ReturnLeaseResultType ReturnLease(
        ::bosdyn::api::ReturnLeaseRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to list all leases for a resource.
    std::shared_future<ListLeasesResultType> ListLeasesAsync(
        bool include_full_lease_info=false,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to issue a list leases request.
    ListLeasesResultType ListLeases(
        bool include_full_lease_info=false,
        const RPCParameters& parameters = RPCParameters());

    // Asynchronous method to retain possession of a lease.
    std::shared_future<RetainLeaseResultType> RetainLeaseAsync(
        ::bosdyn::api::RetainLeaseRequest& request,
        const RPCParameters& parameters = RPCParameters());

    // Synchronous method to issue a retain lease request.
    RetainLeaseResultType RetainLease(::bosdyn::api::RetainLeaseRequest& request,
                                      const RPCParameters& parameters = RPCParameters());


    // Start of ServiceClient overrides.
    QualityOfService GetQualityOfService() const override;
    void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) override;
    void UpdateServiceFrom(
        RequestProcessorChain& request_processor_chain,
        ResponseProcessorChain& response_processor_chain,
        const std::shared_ptr<LeaseWallet>& lease_wallet) override;
    // End of ServiceClient overrides.

    // Get the default service name the Lease service will be registered in the directory with.
    static std::string GetDefaultServiceName() { return s_default_service_name; }

    // Get the lease wallet associated with the lease client.
    std::shared_ptr<LeaseWallet> GetLeaseWallet() { return m_lease_wallet; }

    // Get the default service type for the Lease service that will be registered in the directory.
    static std::string GetServiceType() { return s_service_type; }

 private:
    // Callback function registered for the asynchronous calls to acquire a lease.
    void OnAcquireLeaseComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::AcquireLeaseRequest& request,
        ::bosdyn::api::AcquireLeaseResponse&& response, const grpc::Status& status,
        std::promise<AcquireLeaseResultType> promise);

    // Callback function registered for the asynchronous calls to forcefully take a lease.
    void OnTakeLeaseComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::TakeLeaseRequest& request,
        ::bosdyn::api::TakeLeaseResponse&& response, const grpc::Status& status,
        std::promise<TakeLeaseResultType> promise);

    // Callback function registered for the asynchronous calls to return a lease.
    void OnReturnLeaseComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::ReturnLeaseRequest& request,
        ::bosdyn::api::ReturnLeaseResponse&& response, const grpc::Status& status,
        std::promise<ReturnLeaseResultType> promise);

    // Callback function registered for the asynchronous calls to list all leases and their owners.
    void OnListLeasesComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::ListLeasesRequest& request,
        ::bosdyn::api::ListLeasesResponse&& response, const grpc::Status& status,
        std::promise<ListLeasesResultType> promise);

    // Callback function registered for the asynchronous calls to retain a lease.
    void OnRetainLeaseComplete(
        MessagePumpCallBase* call, const ::bosdyn::api::RetainLeaseRequest& request,
        ::bosdyn::api::RetainLeaseResponse&& response, const grpc::Status& status,
        std::promise<RetainLeaseResultType> promise);


    std::unique_ptr<::bosdyn::api::LeaseService::Stub> m_stub;

    // Default service name for the robot command service.
    static const char* s_default_service_name;

    // The current lease wallet associated with this lease client. This lease wallet maintains the
    // update-to-date leases and the ownership status of each lease.
    std::shared_ptr<LeaseWallet> m_lease_wallet;

    // Default service type for the Lease service.
    static const char* s_service_type;
};

}  // namespace client

}  // namespace bosdyn
