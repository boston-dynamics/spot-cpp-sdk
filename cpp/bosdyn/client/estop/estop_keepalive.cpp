/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/estop/estop_keepalive.h"
#include <mutex>
#include "bosdyn/client/error_codes/error_type_condition.h"
#include "bosdyn/client/error_codes/rpc_error_code.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"

namespace bosdyn {

namespace client {

EstopKeepAlive::EstopKeepAlive(EstopEndpoint* estop_endpoint, ::bosdyn::common::Duration rpc_timeout,
                               ::bosdyn::common::Duration rpc_interval)
    : m_estop_endpoint(estop_endpoint), m_rpc_interval(rpc_interval) {
    if (rpc_timeout == std::chrono::seconds(0)) {
        m_rpc_parameters.timeout = m_estop_endpoint->GetEstopTimeout();
    } else {
        m_rpc_parameters.timeout = rpc_timeout;
    }

    if (rpc_interval == std::chrono::seconds(0)) {
        m_rpc_interval = m_estop_endpoint->GetEstopTimeout() / 3;
    }

    // Add an initial status to the queue.
    this->UpdateStatus(EstopKeepAliveStatus::KEEPALIVE_OK);

    // Do an initial check-in and log any errors that occur. This lets us get
    // a challenge from the E-Stopservice such that we can begin using valid
    // challenge/response pairs for the E-Stop endpoint.
    auto checkin_status = CheckIn();
    if (!checkin_status) {
    }

    // Start the keepalive thread, which will run the periodicCheckin unless
    // the keep_running callback says otherwise.
    m_keepalive_thread = std::thread(&EstopKeepAlive::PeriodicCheckIn, this);
}

EstopKeepAlive::~EstopKeepAlive() {
    m_thread_is_alive = false;
    m_cv.notify_one();
    m_keepalive_thread.join();
}

void EstopKeepAlive::UpdateStatus(EstopKeepAliveStatus status, const std::string& error_msg) {
    std::lock_guard<std::mutex> lock(m_keepalive_mutex);
    // If we're disabled, assume that additional status is irrelevant.
    if (m_status_and_message.first != KEEPALIVE_DISABLED) {
        m_status_and_message = std::make_pair(status, error_msg);
    }
}

::bosdyn::common::Status EstopKeepAlive::CheckIn() {
    std::lock_guard<std::mutex> lock(m_keepalive_mutex);
    return m_estop_endpoint->CheckInAtLevel(m_desired_stop_level, m_rpc_parameters);
}

void EstopKeepAlive::SetStopLevel(::bosdyn::api::EstopStopLevel desired_stop_level) {
    std::lock_guard<std::mutex> lock(m_keepalive_mutex);
    if (m_desired_stop_level == desired_stop_level) return;

    // Change the stop level and wake the background thread.
    m_desired_stop_level = desired_stop_level;
    m_cv.notify_one();
}

void EstopKeepAlive::StopKeepAliveThread(const std::string& error_msg) {
    m_thread_is_alive = false;
    m_cv.notify_one();
    this->UpdateStatus(EstopKeepAliveStatus::KEEPALIVE_DISABLED, error_msg);
}

void EstopKeepAlive::SendError(const std::string& error_msg, EstopKeepAliveStatus disable_status) {
    this->UpdateStatus(disable_status, error_msg);
    if (disable_status == EstopKeepAliveStatus::KEEPALIVE_DISABLED) {
        this->StopKeepAliveThread(error_msg);
    }
}

void EstopKeepAlive::SendOk() {
    this->UpdateStatus(EstopKeepAliveStatus::KEEPALIVE_OK);
}

void EstopKeepAlive::PeriodicCheckIn() {
    while (this->m_thread_is_alive) {
        // Include the time it takes to execute keep_running, in case it takes a significant
        // portion of our check in period.
        auto exec_start_ns = std::chrono::steady_clock::now().time_since_epoch();

        // Attempt a check-in to the robot's estop-service.
        ::bosdyn::common::Status checkin_status = this->CheckIn();
        if (checkin_status.code() == ErrorTypeCondition::RPCError) {
            // ::bosdyn::common::Status shows a GRPC related error.
            std::string error_msg;
            if (checkin_status.code() == RPCErrorCode::TimedOutError) {
                error_msg = "RPC took longer than " + std::to_string(m_rpc_parameters.timeout.count()) + "[ms].";
            } else {
                error_msg =
                    "Transport exception during check-in: " + std::to_string(checkin_status) +
                    "\n (resuming check-in)";
            }
            this->SendError(error_msg);
        } else if (checkin_status.code() == ErrorTypeCondition::SDKError &&
                   !(checkin_status.code() == SDKErrorCode::Success)) {
            // ::bosdyn::common::Status shows an SDK error.
            this->SendError(checkin_status.message());
        } else if (checkin_status.code() ==
                   ::bosdyn::api::EstopCheckInResponse::STATUS_ENDPOINT_UNKNOWN) {
            // E-Stop checkin responded that it does not know the endpoint for the check-in request.
            // Disable the keep-alive in response.
            this->SendError(checkin_status.message(), EstopKeepAliveStatus::KEEPALIVE_DISABLED);
        } else {
            if (!checkin_status) {
                // Some other response error has occurred, but not one worthy of disabling the E-Stop
                // keep-alive.
                this->SendError(checkin_status.message());
            } else {
                // No notable errors!
                this->SendOk();
            }
        }

        // How long should the thread wait if it isn't stopped by an external application.
        std::chrono::nanoseconds exec_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch() - exec_start_ns);
        auto wait_time = this->m_rpc_interval - exec_ns;

        // Block and wait for the stop signal. If we receive it within the check-in period,
        // leave the loop. This check must be at the end of the loop!
        // Wait for m_rpc_interval time minus the RPC processing time.
        // (values < 0 are OK and will return immediately)
        std::unique_lock<std::mutex> lock(this->m_keepalive_mutex);
        this->m_cv.wait_for(lock, wait_time);
    }
}

}  // namespace client

}  // namespace bosdyn
