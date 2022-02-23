/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "lease_wallet.h"

namespace bosdyn {

namespace client {

LeaseWallet::LeaseWallet(const std::string& client_name) : m_client_name(client_name) {}

void LeaseWallet::AddLease(const std::string& resource, const Lease& lease, SubLease lease_option) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (lease_option == SubLease::kNoSubLease || !lease.IsLeaseSelfOwned()) {
        m_lease_map[resource] = lease;
    } else {
        m_lease_map[resource] = lease.CreateSublease(m_client_name);
    }
}

void LeaseWallet::AddLease(const Lease& lease, SubLease lease_option) {
    AddLease(lease.GetResource(), lease, lease_option);
}

void LeaseWallet::RemoveLease(const std::string& resource) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_lease_map.erase(resource);
}

::bosdyn::common::Status LeaseWallet::FailLease(const Lease& lease) {
    // Invalid leases should never be in the wallet.
    if (!lease.IsValid()) {
        return ::bosdyn::common::Status(
            LeaseWalletErrorCode::LeaseInvalidError,
            "LeaseWallet could not fail the lease because it is invalid. This "
            "should never happen for a lease in the wallet.");
    }
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_lease_map.find(lease.GetProto().resource());
        if (it == m_lease_map.end()) {
            return ::bosdyn::common::Status(
                LeaseWalletErrorCode::ResourceNotInWalletError,
                "LeaseWallet could not fail the lease because the lease resource "
                "is not in the wallet");
        }
        // If the "lease" field differs from the current lease in the wallet, then do
        // not mutate the wallet. This can happen when the lease in the wallet is updated while an
        // outbound request completes with an older version of the lease.
        if (it->second.Compare(lease) != Lease::CompareResult::SAME) {
            return ::bosdyn::common::Status(
                LeaseWalletErrorCode::GenericLeaseError,
                "LeaseWallet could not fail the lease because the input lease is "
                "different from the lease in the wallet.");
        }
        m_lease_map.erase(it);
        return ::bosdyn::common::Status(LeaseWalletErrorCode::Success);
    }
}

Result<Lease> LeaseWallet::GetLease(const std::string& resource) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_lease_map.find(resource);
    if (it == m_lease_map.end()) {
        return {::bosdyn::common::Status(
            LeaseWalletErrorCode::ResourceNotInWalletError,
            "LeaseWallet could not find the lease because the lease resource is not in the wallet"),
            {}};
    }
    return {::bosdyn::common::Status(LeaseWalletErrorCode::Success), it->second};
}

Result<LeaseProto> LeaseWallet::GetLeaseProto(const std::string& resource) {
    auto lease_result = this->GetLease(resource);
    if (lease_result.status) {
        // Successfully found the lease. Return the proto for that Lease class.
        return {::bosdyn::common::Status(LeaseWalletErrorCode::Success),
                lease_result.response.GetProto()};
    }
    // Something failed with finding the lease. Return a nullptr.
    return {lease_result.status, {}};
}

Result<Lease> LeaseWallet::GetOwnedLease(const std::string& resource) {
    auto lease_result = this->GetLease(resource);
    if (!lease_result.status) {
        // Regular GetLease method returned an error status. Propagate that failure status.
        return {lease_result.status, {}};
    }
    // Check if the lease is actually owned by the lease wallet.
    if (lease_result.response.IsLeaseSelfOwned()) {
        return {::bosdyn::common::Status(LeaseWalletErrorCode::Success),
                std::move(lease_result.response)};
    } else {
        return {::bosdyn::common::Status(
            LeaseWalletErrorCode::ResourceNotOwnedError,
            "LeaseWallet could not find a self-owned lease for this resource."),
            {}};
    }
}

Result<LeaseProto> LeaseWallet::GetOwnedLeaseProto(const std::string& resource) {
    auto lease_result = this->GetOwnedLease(resource);
    if (lease_result.status) {
        // Successfully found the lease. Return the proto for that Lease class.
        return {::bosdyn::common::Status(LeaseWalletErrorCode::Success),
                lease_result.response.GetProto()};
    }
    // Failed to find an owned lease, propagate that failure with a nullptr for the LeaseProto.
    return {lease_result.status, {}};
}

std::vector<Lease> LeaseWallet::GetAllLeases() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<Lease> ret;
    for (auto key_value_pair : m_lease_map) {
        ret.push_back(key_value_pair.second);
    }
    return ret;
}

std::vector<std::string> LeaseWallet::GetAllOwnedResources() const {
    std::vector<std::string> owned_resources;
    for (auto resc_lease_pair : m_lease_map) {
        if (resc_lease_pair.second.IsLeaseSelfOwned()) {
            owned_resources.push_back(resc_lease_pair.first);
        }
    }
    return owned_resources;
}

Result<Lease> LeaseWallet::AdvanceLease(const std::string& resource) {
    auto old_lease_result = this->GetOwnedLease(resource);
    if (!old_lease_result) {
        return old_lease_result;
    }
    Lease new_lease;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        new_lease = old_lease_result.response.Increment();
        // Remove the old lease and then update the map with the new lease.
        m_lease_map.erase(resource);
        m_lease_map[resource] = new_lease;
    }
    return {::bosdyn::common::Status(LeaseWalletErrorCode::Success), std::move(new_lease)};
}

::bosdyn::common::Status LeaseWallet::OnLeaseUseResult(
    const ::bosdyn::api::LeaseUseResult& lease_use_result) {
    std::string resource = lease_use_result.attempted_lease().resource();
    Lease found_lease;
    auto lease_found_result = this->GetLease(resource);
    if (!lease_found_result.status) {
        // Couldn't find a lease for this resource.
        return lease_found_result.status;
    }
    // Update the lease with the lease use results.
    lease_found_result.response.UpdateFromLeaseUseResult(lease_use_result);
    // Add the updated lease into the lease wallet for that resource.
    this->AddLease(resource, lease_found_result.response, SubLease::kNoSubLease);
    return ::bosdyn::common::Status(LeaseWalletErrorCode::Success);
}

std::string LeaseWallet::GetClientName() const { return m_client_name; }

}  // namespace client

}  // namespace bosdyn
