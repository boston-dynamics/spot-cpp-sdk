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
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include <bosdyn/api/estop.pb.h>

#include "bosdyn/common/status.h"
#include "bosdyn/client/estop/estop_client.h"
#include "bosdyn/client/estop/estop_endpoint.h"

namespace bosdyn {

namespace client {

enum EstopKeepAliveStatus { KEEPALIVE_OK, KEEPALIVE_ERROR, KEEPALIVE_DISABLED };

// Wraps an EstopEndpoint to do periodic check-ins, keeping software E-Stop from timing out. This
// is intended to be the common implementation of both periodic checking-in and one-time
// check-ins.
class EstopKeepAlive {
 public:
    // Constructor for the keepalive thread. This requires the E-Stop endpoint to do check-ins for.
    // Additionally, an rpc timeout seconds can be passed, but uses the endpoint's E-Stop timeout by
    // default. The rpc interval time can also be passed, but will use the (estop_timeout) / 3
    // seconds if nothing is provided.
    explicit EstopKeepAlive(
        EstopEndpoint* estop_endpoint,
        ::bosdyn::common::Duration rpc_timeout_seconds = std::chrono::seconds(0),
        ::bosdyn::common::Duration rpc_interval_time = std::chrono::seconds(0));

    // Destructor for the keepalive thread, to shutdown the thread nicely.
    ~EstopKeepAlive();

    // Check that the keepalive thread is still running.
    bool IsAlive() const { return m_thread_is_alive; }

    // Manually stop the E-Stop keepalive thread.
    void StopKeepAliveThread(const std::string& error_msg = "");

    // Get the E-Stop endpoint that this keepalive preserves.
    EstopEndpoint* GetEstopEndpoint() { return m_estop_endpoint; }

    // Get the E-Stop client for the endpoint this keepalive preserves.
    EstopClient* GetEstopClient() { return m_estop_endpoint->GetEstopClient(); }

    // Get the latest status.
    std::pair<EstopKeepAliveStatus, std::string> GetLatestEstopStatus() {
        std::lock_guard<std::mutex> lock(m_keepalive_mutex);
        return m_status_and_message;
    }

    // Adjust the stop level that is being sent.
    void SetStopLevel(::bosdyn::api::EstopStopLevel desired_stop_level);

    // The main component of the keepalive class is std::thread, which is not copyable.
    // Therefore, the keepalive class is also not movable or copyable.
    EstopKeepAlive(const EstopKeepAlive&) = delete;
    EstopKeepAlive operator=(const EstopKeepAlive&) = delete;

 private:
    // The E-Stop endpoint to keepalive with periodic check-ins.
    EstopEndpoint* m_estop_endpoint;

    // Keepalive thread.
    std::thread m_keepalive_thread;

    // Lock for the condition variable
    std::mutex m_keepalive_mutex;

    // Condition Variable for checking if the thread should still be sleeping or be stopped.
    std::condition_variable m_cv;

    // Boolean indicating it the thread is still running or if it has returned.
    std::atomic<bool> m_thread_is_alive = {true};

    // The desired stop level for the E-Stop. This is the kind of emergency stop that will be
    // performed. By default, it is set to None.
    ::bosdyn::api::EstopStopLevel m_desired_stop_level =
        ::bosdyn::api::EstopStopLevel::ESTOP_LEVEL_NONE;

    // Duration in seconds between liveness checks. This defaults to 2 seconds, but can be updated.
    ::bosdyn::common::Duration m_rpc_interval;

    // The RPC will timeout after this many seconds. Store this information in the RPC Parameters
    // message to be passed to the estop client.
    RPCParameters m_rpc_parameters;

    // A queue of all the recent E-Stop keepalive statuses and any associated error messages with
    // the status.
    std::pair<EstopKeepAliveStatus, std::string> m_status_and_message;

    // Check-in to maintain the desired E-Stop system level.
    ::bosdyn::common::Status CheckIn();

    // Main loop for the keepalive thread, which will send E-Stop API CheckIn messages to robot
    // E-Stop system in loop, then wait a fixed amount of time.
    void PeriodicCheckIn();

    // Add a new status to the status queue.
    void UpdateStatus(EstopKeepAliveStatus status, const std::string& error_msg = "");

    // Handle an error message from the check-in process; optionally disable the keepalive.
    void SendError(const std::string& error_msg,
                   EstopKeepAliveStatus disable_status = EstopKeepAliveStatus::KEEPALIVE_ERROR);

    // Handle an OK check-in state.
    void SendOk();
};

}  // namespace client

}  // namespace bosdyn
