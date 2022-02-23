/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include "bosdyn/client/error_codes/lease_wallet_error_code.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"
#include "bosdyn/common/status.h"
#include "bosdyn/client/lease/lease.h"
#include "bosdyn/client/lease/lease_error_codes.h"
#include "bosdyn/client/lease/lease_wallet.h"
#include "bosdyn/common/assert_precondition.h"

namespace bosdyn {

namespace client {

// Only call with "lease" field.
template <class RpcRequestWithLease>
::bosdyn::common::Status ProcessRequestWithLease(RpcRequestWithLease* request_with_lease,
                                                 LeaseWallet* lease_wallet,
                                                 const std::string& resource) {
    BOSDYN_ASSERT_PRECONDITION(lease_wallet != nullptr,
                               "No lease wallet provided for the request.");

    if (request_with_lease->has_lease()) {
        // Skip any processing if a lease is already set within the request.
        return ::bosdyn::common::Status(LeaseWalletErrorCode::Success);
    }

    if (resource.empty()) {
        // No lease resource requested, so return success and skip any processing.
        return ::bosdyn::common::Status(LeaseWalletErrorCode::Success);
    }

    // The lease field is a singular lease field and we will add an updated lease for the
    // desired resource.
    auto adv_lease_result = lease_wallet->AdvanceLease(resource);
    if (adv_lease_result) {
        // Successfully advanced lease for this resource. Add it to the request.
        *request_with_lease->mutable_lease() = adv_lease_result.response.GetProto();
        return ::bosdyn::common::Status(LeaseWalletErrorCode::Success);
    } else {
        // Failed to advance lease for this resource. This can only happen if the resource's
        // lease does not already exist in the wallet.
        return adv_lease_result.status.Chain("Cannot advance the lease for " + resource);
    }
}

template <class RpcRequestMultiLease>
::bosdyn::common::Status ProcessRequestWithMultipleLeases(
    RpcRequestMultiLease* request_with_lease, LeaseWallet* lease_wallet,
    const std::vector<std::string>& resource_list) {
    BOSDYN_ASSERT_PRECONDITION(lease_wallet != nullptr,
                               "No lease wallet provided for the request.");

    if (request_with_lease->leases_size() >= resource_list.size()) {
        // User already attached the necessary leases. Don't try to add more.
        return ::bosdyn::common::Status(LeaseWalletErrorCode::Success);
    }

    for (const std::string& resc : resource_list) {
        auto adv_lease_result = lease_wallet->AdvanceLease(resc);
        if (adv_lease_result) {
            // Successfully advanced lease for this resource. Add it to the request.
            *request_with_lease->add_leases() = adv_lease_result.response.GetProto();
        } else {
            // Failed to advance lease for this resource. This can only happen if the resource's
            // lease does not already exist in the wallet.
            return adv_lease_result.status.Chain("Cannot advance the lease for " + resc);
        }
    }
    return ::bosdyn::common::Status(LeaseWalletErrorCode::Success);
}

template <class RpcResponseWithLeaseResult>
::bosdyn::common::Status ProcessResponseWithLeaseUseResult(
    const RpcResponseWithLeaseResult& full_response, LeaseWallet* lease_wallet) {
    BOSDYN_ASSERT_PRECONDITION(lease_wallet != nullptr,
                               "No lease wallet provided for the request.");

    if (!full_response.has_lease_use_result()) {
        // No lease use results to process.
        return ::bosdyn::common::Status(::bosdyn::api::LeaseUseResult::STATUS_OK);
    }
    // Update the lease wallet with the lease use result proto and respond with the status.
    // NOTE: The OnLeaseUseResult can only fail if somehow we lose the lease from the wallet
    // between the request being sent and receiving the response. Ignoring the error (and nodiscard
    // warnings) in this case because we want the regular LeaseUseResult error returned.
    lease_wallet->OnLeaseUseResult(full_response.lease_use_result()).IgnoreError();
    // Return the lease use result status.
    return ::bosdyn::common::Status(full_response.lease_use_result().status());
}

template <class RpcResponseMultiLeaseResults>
::bosdyn::common::Status ProcessResponseForMultipleLease(
    const RpcResponseMultiLeaseResults& full_response, LeaseWallet* lease_wallet) {
    BOSDYN_ASSERT_PRECONDITION(lease_wallet != nullptr,
                               "No lease wallet provided for the request.");

    ::bosdyn::api::LeaseUseResult::Status lease_use_status = ::bosdyn::api::LeaseUseResult::STATUS_OK;
    for (const auto& lease_use_res : full_response.lease_use_results()) {
        // Update the lease wallet with the lease use result proto for each resource.
        // NOTE: The OnLeaseUseResult can only fail if somehow we lose the lease from the wallet
        // between the request being sent and receiving the response. Ignoring the error (and nodiscard
        // warnings) in this case because we want the regular LeaseUseResult error returned.
        lease_wallet->OnLeaseUseResult(lease_use_res).IgnoreError();
        if (lease_use_res.status() != ::bosdyn::api::LeaseUseResult::STATUS_OK) {
            lease_use_status = lease_use_res.status();
        }
    }
    // Returns an error status if one exists, otherwise just returns STATUS_OK.
    return ::bosdyn::common::Status(lease_use_status);
}

}  // namespace client

}  // namespace bosdyn
