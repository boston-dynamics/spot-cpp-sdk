/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/lease.pb.h>
#include "lease.h"
#include "bosdyn/client/service_client/result.h"
#include "bosdyn/client/error_codes/lease_wallet_error_code.h"
#include "bosdyn/common/status.h"

#include <mutex>

namespace bosdyn {

namespace client {

class LeaseWallet {
 public:
    explicit LeaseWallet(const std::string& client_name);
    ~LeaseWallet() = default;

    enum class SubLease {
        kDefault,
        kNoSubLease,
    };

    // Set the lease for a specific resource. Always guaranteed to succeed.
    // Will automatically sublease a self-owned lease.  This can be overridden with lease_option.
    void AddLease(const std::string& resource, const Lease& lease,
                  SubLease lease_option = SubLease::kDefault);

    // Set the lease, reading the resource out of the lease. Always guaranteed to succeed.
    // Will automatically sublease a self-owned lease.  This can be overridden with lease_option.
    void AddLease(const Lease& lease, SubLease lease_option = SubLease::kDefault);

    // Remove a Lease for a specific resource, if one exists.
    void RemoveLease(const std::string& resource);

    // Update the lease state based on result of using the lease.
    ::bosdyn::common::Status OnLeaseUseResult(const ::bosdyn::api::LeaseUseResult& lease_use_result);

    // Fail the current lease for a resource, based on server response in LeaseUseResult.
    // If 'lease' is the same as the current lease in the wallet, record why the Lease failed and
    // return true.
    // If 'lease' differs from the current lease or is invalid, return false. This situation can
    // happen when a command was issued with an older version of a lease, and a newer lease is
    // stored in the wallet.
    ::bosdyn::common::Status FailLease(const Lease& lease);

    // Get the lease for a resource, if one exists and this lease wallet is the owner of the lease.
    Result<Lease> GetOwnedLease(const std::string& resource);
    Result<LeaseProto> GetOwnedLeaseProto(const std::string& resource);

    // Get the lease for a resource, if one exists. If a lease exists, true is returned and |lease|
    // is populated. If a lease does not exist, false is returned and |lease| is not populated.
    Result<Lease> GetLease(const std::string& resource);
    Result<LeaseProto> GetLeaseProto(const std::string& resource);

    // Return all Leases currently being managed by the wallet.
    std::vector<Lease> GetAllLeases() const;

    // Return all lease resources managed (and currently owned) by the lease wallet.
    std::vector<std::string> GetAllOwnedResources() const;

    // Advance the lease for this resource in the lease wallet and return it.
    bosdyn::client::Result<Lease> AdvanceLease(const std::string& resource);

    // Get the client name associated with this wallet.
    std::string GetClientName() const;

 private:
    mutable std::mutex m_mutex;
    typedef std::map<std::string, Lease> LeaseMap;
    LeaseMap m_lease_map;
    std::string m_client_name;
};

}  // namespace client

}  // namespace bosdyn
