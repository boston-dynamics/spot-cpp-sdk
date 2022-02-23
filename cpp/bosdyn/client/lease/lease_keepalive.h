/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <bosdyn/api/lease.pb.h>

#include "bosdyn/client/error_codes/lease_wallet_error_code.h"
#include "bosdyn/common/status.h"
#include "bosdyn/client/lease/lease_client.h"
#include "bosdyn/client/lease/lease_resources.h"
#include "bosdyn/client/lease/lease_wallet.h"

namespace bosdyn {

namespace client {

typedef ::bosdyn::api::Lease LeaseProto;

/**
 * This is a function used by the LeaseKeepAlive and will get called whenever a RetainLease RPC
 * does not succeed. The function takes the result and status of the RPC and a pointer to the
 * lease keep alive class which is running the background thread as inputs, and the application
 * can respond to the failure (such as stopping the keepalive thread) within the keepalive thread.
 *
 * The input result can have the following error codes:
 *    RpcErrorCodes,
 *    CommonError::Code,
 *    LeaseUseResult::Status,
 *    LeaseWalletErrorCode::ResourceNotInWalletError
 */
template<class LeaseKeepAliveClass>
using OnRetainLeaseFailure = std::function<void(const Result<::bosdyn::api::RetainLeaseResponse>& retain_lease_result, LeaseKeepAliveClass* lease_keep_alive)>;

// LeaseKeepAlive issues lease liveness checks on a background thread.
// The robot's lease system expects lease holders to check in at a regular
// cadence. If the check-ins do not happen, the robot will treat it as a
// communications loss. Typically this will result in the robot stopping,
// powering off, and the lease holder getting their lease revoked.
// Using a LeaseKeepAlive object hides most of the details of issuing the
// lease liveness check. Developers can also manage liveness checks directly
// by using the retain_lease methods on the LeaseClient object.
class LeaseKeepAlive {
 public:
    // Constructor for the keepalive thread. This requires an argument for the lease_client to be
    // provided, since the keepalive thread's job is to make retain lease rpc's. Additionally, the
    // lease wallet can (optionally) be passed; if nothing is provided, then the lease wallet of the
    // client will be used. Finally, the resource that the keepalive should be maintaining can
    // (optionally) be passed. If it is not passed, the thread defaults to preserving the "body".
    explicit LeaseKeepAlive(LeaseClient* lease_client,
                            std::shared_ptr<LeaseWallet> lease_wallet = nullptr,
                            const std::string& resource = ::bosdyn::client::kBodyResource,
                            ::bosdyn::common::Duration rpc_interval_time = std::chrono::seconds(2),
                            OnRetainLeaseFailure<LeaseKeepAlive> on_failure_fn = nullptr);

    // Destructor for the keepalive thread, to shutdown the thread nicely.
    ~LeaseKeepAlive();

    // Function to reset the rpc interval time.
    void SetRpcInterval(::bosdyn::common::Duration interval);

    // Check that the keepalive thread is still running.
    bool IsAlive() const { return m_thread_is_alive; }

    // Manually stop the lease keepalive thread.
    void StopKeepAliveThread();

    // Get the lease wallet for the keepalive class.
    std::shared_ptr<LeaseWallet> GetLeaseWallet() { return m_lease_wallet; }

    // Get the resource this keepalive maintains.
    const std::string& GetKeepAliveResource() { return m_resource; }

    // The main component of the keepalive class is std::thread, which is not copyable.
    // Therefore, the keepalive class is also not moveable or copyable.
    LeaseKeepAlive(const LeaseKeepAlive&) = delete;
    LeaseKeepAlive operator=(const LeaseKeepAlive&) = delete;

 private:
    // The list of resources for which we need to keep leases alive for (do liveness checks for each
    // lease). This will default to only the "body" resource.
    const std::string m_resource;

    // The LeaseClient object to issue requests on.
    LeaseClient* m_lease_client = nullptr;

    // The LeaseWallet to retrieve current leases from, and to handle any bad LeaseUseResults from.
    // If not specified, the lease_client's lease_wallet will be used.
    std::shared_ptr<LeaseWallet> m_lease_wallet;

    // Duration in seconds between liveness checks. This defaults to 2 seconds, but can be updated.
    ::bosdyn::common::Duration m_rpc_interval;

    // Keepalive thread.
    std::thread m_keepalive_thread;

    // Main loop for the keepalive thread, which will try to retain the lease for the specified
    // resource, and then wait for a fixed amount of time.
    void PeriodicCheckIn();

    // Lock for the condition variable
    std::mutex m_keepalive_mutex;

    // Condition Variable for checking if the thread should still be sleeping or be stopped.
    std::condition_variable m_cv;

    // Boolean indicating it the thread is still running or if it has returned.
    bool m_thread_is_alive = true;

    // Function to be called when the retain lease RPC does not succeed.
    OnRetainLeaseFailure<LeaseKeepAlive> m_on_retain_lease_failure_func = nullptr;
};

}  // namespace client

}  // namespace bosdyn
