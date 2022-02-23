/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/lease/lease_keepalive.h"
#include <bosdyn/api/lease.pb.h>
#include "bosdyn/common/time.h"

namespace bosdyn {

namespace client {

LeaseKeepAlive::LeaseKeepAlive(LeaseClient* lease_client,
                               std::shared_ptr<LeaseWallet> lease_wallet,
                               const std::string& resource,
                               ::bosdyn::common::Duration rpc_interval_time,
                               OnRetainLeaseFailure<LeaseKeepAlive> on_failure_fn)
    : m_resource(resource),
      m_lease_client(lease_client),
      m_rpc_interval(rpc_interval_time),
      m_on_retain_lease_failure_func(on_failure_fn) {
    // Set the lease wallet. If none is provided, then set the lease wallet as the one
    // from the client.
    if (lease_wallet) {
        m_lease_wallet = lease_wallet;
    } else {
        // Use the lease wallet from the lease client if no other lease is provided.
        m_lease_wallet = m_lease_client->GetLeaseWallet();
    }

    // Start the keepalive thread, which will run the periodicCheckin unless
    // the keep_running callback says otherwise.
    m_keepalive_thread = std::thread(&LeaseKeepAlive::PeriodicCheckIn, this);
}

LeaseKeepAlive::~LeaseKeepAlive() {
    m_thread_is_alive = false;
    m_cv.notify_one();
    m_keepalive_thread.join();
}

void LeaseKeepAlive::SetRpcInterval(::bosdyn::common::Duration interval) {
    m_rpc_interval = interval;
}

void LeaseKeepAlive::StopKeepAliveThread() {
    m_thread_is_alive = false;
    m_cv.notify_one();
}

void LeaseKeepAlive::PeriodicCheckIn() {
    while (this->m_thread_is_alive) {
        // Include the time it takes to execute keep_running, in case it takes a significant
        // portion of our check in period.
        auto exec_start_ns = std::chrono::steady_clock::now().time_since_epoch();
        auto found_lease = m_lease_wallet->GetLease(m_resource);
        if (found_lease) {
            // Call the client to retain the lease.
            ::bosdyn::api::RetainLeaseRequest req;
            req.mutable_lease()->CopyFrom(found_lease.response.GetProto());
            auto future = m_lease_client->RetainLeaseAsync(req);
            while (std::future_status::ready != future.wait_for(std::chrono::milliseconds(100))) {
                if (!m_thread_is_alive) {
                    return;
                }
            }
            auto retain_lease_result = future.get();
            if (!retain_lease_result) {
                fprintf(stderr, "LeaseKeepAlive RetainLeases RPC failed: '%s'\n",
                        retain_lease_result.status.DebugString().c_str());
                if (m_on_retain_lease_failure_func) {
                    m_on_retain_lease_failure_func(retain_lease_result, this);
                }
            }
        } else {
            // Provide the lease wallet error code to the on failure function. Use an empty retain
            ::bosdyn::api::RetainLeaseResponse resp;
            if (m_on_retain_lease_failure_func) {
                m_on_retain_lease_failure_func({found_lease.status, resp}, this);
            }
        }

        // How long should the thread wait if it isn't stopped by an external application.
        std::chrono::nanoseconds exec_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch() - exec_start_ns);
        auto wait_time = this->m_rpc_interval - exec_ns;

        // Block and wait for the stop signal. If we receive it within the check-in period,
        // leave the loop. This check must be at the end of the loop!
        // Wait for m_rpc_interval_milliseconds minus the RPC processing time.
        // (values < 0 are OK and will return immediately)
        std::unique_lock<std::mutex> lock(this->m_keepalive_mutex);
        this->m_cv.wait_for(lock, wait_time);
    }
}

}  // namespace client

}  // namespace bosdyn
