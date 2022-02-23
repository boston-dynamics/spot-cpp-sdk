/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "lease_client.h"

#include "bosdyn/client/error_codes/rpc_error_code.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"
#include "bosdyn/client/lease/lease_processors.h"

using namespace std::placeholders;

namespace bosdyn {

namespace client {

const char* LeaseClient::s_default_service_name = "lease";

const char* LeaseClient::s_service_type = "bosdyn.api.LeaseService";

std::shared_future<AcquireLeaseResultType> LeaseClient::AcquireLeaseAsync(
    const std::string& resource, const RPCParameters& parameters) {
    std::promise<AcquireLeaseResultType> response;
    std::shared_future<AcquireLeaseResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::AcquireLeaseRequest request;
    request.set_resource(resource);
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::AcquireLeaseRequest, ::bosdyn::api::AcquireLeaseResponse,
                          ::bosdyn::api::AcquireLeaseResponse>(
            request,
            std::bind(&::bosdyn::api::LeaseService::Stub::AsyncAcquireLease, m_stub.get(), _1, _2, _3),
            std::bind(&LeaseClient::OnAcquireLeaseComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

void LeaseClient::OnAcquireLeaseComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::AcquireLeaseRequest& request,
    ::bosdyn::api::AcquireLeaseResponse&& response, const grpc::Status& status,
    std::promise<AcquireLeaseResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::AcquireLeaseResponse>(
        status, response, response.status());

    if (m_lease_wallet) {
        if (ret_status) {
            // Add the lease into the wallet if the acquire lease request was a success.
            m_lease_wallet->AddLease(request.resource(), ::bosdyn::client::Lease(response.lease()));
        }
    }

    promise.set_value({ret_status, std::move(response)});
}

AcquireLeaseResultType LeaseClient::AcquireLease(
    const std::string& resource, const RPCParameters& parameters) {
    return AcquireLeaseAsync(resource, parameters).get();
}

std::shared_future<TakeLeaseResultType> LeaseClient::TakeLeaseAsync(
    const std::string& resource, const RPCParameters& parameters) {
    std::promise<TakeLeaseResultType> response;
    std::shared_future<TakeLeaseResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::TakeLeaseRequest request;
    request.set_resource(resource);
    MessagePumpCallBase* one_time = InitiateAsyncCall<::bosdyn::api::TakeLeaseRequest, ::bosdyn::api::TakeLeaseResponse,
                                                      ::bosdyn::api::TakeLeaseResponse>(
        request, std::bind(&::bosdyn::api::LeaseService::Stub::AsyncTakeLease, m_stub.get(), _1, _2, _3),
        std::bind(&LeaseClient::OnTakeLeaseComplete, this, _1, _2, _3, _4, _5),
        std::move(response), parameters);

    return future;
}

void LeaseClient::OnTakeLeaseComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::TakeLeaseRequest& request,
    ::bosdyn::api::TakeLeaseResponse&& response, const grpc::Status& status,
    std::promise<TakeLeaseResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::TakeLeaseResponse>(status, response,
                                                                                 response.status());

    if (m_lease_wallet) {
        if (ret_status) {
            // Add the lease into the wallet if the take lease request was a success.
            m_lease_wallet->AddLease(request.resource(), ::bosdyn::client::Lease(response.lease()));
        }
    }

    promise.set_value({ret_status, std::move(response)});
}

TakeLeaseResultType LeaseClient::TakeLease(
    const std::string& resource, const RPCParameters& parameters) {
    return TakeLeaseAsync(resource, parameters).get();
}

std::shared_future<ReturnLeaseResultType> LeaseClient::ReturnLeaseAsync(
    ::bosdyn::api::ReturnLeaseRequest& request, const RPCParameters& parameters) {
    std::promise<ReturnLeaseResultType> response;
    std::shared_future<ReturnLeaseResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::ReturnLeaseRequest, ::bosdyn::api::ReturnLeaseResponse,
                          ::bosdyn::api::ReturnLeaseResponse>(
            request,
            std::bind(&::bosdyn::api::LeaseService::Stub::AsyncReturnLease, m_stub.get(), _1, _2, _3),
            std::bind(&LeaseClient::OnReturnLeaseComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

void LeaseClient::OnReturnLeaseComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::ReturnLeaseRequest& request,
    ::bosdyn::api::ReturnLeaseResponse&& response, const grpc::Status& status,
    std::promise<ReturnLeaseResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::ReturnLeaseResponse>(
        status, response, response.status());

    // Remove the lease from the wallet regardless of the status because all statuses indicated
    // it is no longer being managed by this client somehow.
    m_lease_wallet->RemoveLease(request.lease().resource());

    promise.set_value({ret_status, std::move(response)});
}

ReturnLeaseResultType LeaseClient::ReturnLease(
    ::bosdyn::api::ReturnLeaseRequest& request, const RPCParameters& parameters) {
    return ReturnLeaseAsync(request, parameters).get();
}

std::shared_future<ListLeasesResultType> LeaseClient::ListLeasesAsync(
    bool include_full_lease_info, const RPCParameters& parameters) {
    std::promise<ListLeasesResultType> response;
    std::shared_future<ListLeasesResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    ::bosdyn::api::ListLeasesRequest request;
    request.set_include_full_lease_info(include_full_lease_info);
    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::ListLeasesRequest, ::bosdyn::api::ListLeasesResponse,
                          ::bosdyn::api::ListLeasesResponse>(
            request, std::bind(&::bosdyn::api::LeaseService::Stub::AsyncListLeases, m_stub.get(), _1, _2, _3),
            std::bind(&LeaseClient::OnListLeasesComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

void LeaseClient::OnListLeasesComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::ListLeasesRequest& request,
    ::bosdyn::api::ListLeasesResponse&& response, const grpc::Status& status,
    std::promise<ListLeasesResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::ListLeasesResponse>(
        status, response, SDKErrorCode::Success);

    promise.set_value({ret_status, std::move(response)});
}

ListLeasesResultType LeaseClient::ListLeases(
    bool include_full_lease_info, const RPCParameters& parameters) {
    return ListLeasesAsync(include_full_lease_info, parameters).get();
}

std::shared_future<RetainLeaseResultType> LeaseClient::RetainLeaseAsync(
    ::bosdyn::api::RetainLeaseRequest& request, const RPCParameters& parameters) {
    std::promise<RetainLeaseResultType> response;
    std::shared_future<RetainLeaseResultType> future = response.get_future();
    BOSDYN_ASSERT_PRECONDITION(m_stub != nullptr, "Stub for service is unset!");

    MessagePumpCallBase* one_time =
        InitiateAsyncCall<::bosdyn::api::RetainLeaseRequest, ::bosdyn::api::RetainLeaseResponse,
                          ::bosdyn::api::RetainLeaseResponse>(
            request,
            std::bind(&::bosdyn::api::LeaseService::Stub::AsyncRetainLease, m_stub.get(), _1, _2, _3),
            std::bind(&LeaseClient::OnRetainLeaseComplete, this, _1, _2, _3, _4, _5),
            std::move(response), parameters);

    return future;
}

void LeaseClient::OnRetainLeaseComplete(
    MessagePumpCallBase* call, const ::bosdyn::api::RetainLeaseRequest& request,
    ::bosdyn::api::RetainLeaseResponse&& response, const grpc::Status& status,
    std::promise<RetainLeaseResultType> promise) {
    ::bosdyn::common::Status ret_status = ProcessResponseAndGetFinalStatus<::bosdyn::api::RetainLeaseResponse>(
        status, response, response.lease_use_result().status());

    promise.set_value({ret_status, std::move(response)});
}

RetainLeaseResultType LeaseClient::RetainLease(
    ::bosdyn::api::RetainLeaseRequest& request, const RPCParameters& parameters) {
    return RetainLeaseAsync(request, parameters).get();
}


ServiceClient::QualityOfService LeaseClient::GetQualityOfService() const {
    return QualityOfService::NORMAL;
}

void LeaseClient::SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) {
    m_stub.reset(new ::bosdyn::api::LeaseService::Stub(channel));
}

void LeaseClient::UpdateServiceFrom(RequestProcessorChain& request_processor_chain,
                                    ResponseProcessorChain& response_processor_chain,
                                    const std::shared_ptr<LeaseWallet>& lease_wallet) {
    m_request_processor_chain = request_processor_chain;
    m_response_processor_chain = response_processor_chain;
    m_lease_wallet = lease_wallet;
}

}  // namespace client

}  // namespace bosdyn
