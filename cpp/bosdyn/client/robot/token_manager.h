/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <functional>
#include <memory>
#include <mutex>

#include <bosdyn/api/auth.pb.h>
#include "robot.h"
#include "bosdyn/client/error_callback/error_callback_result.h"
#include "bosdyn/client/util/periodic_thread_helper.h"

namespace bosdyn {

namespace client {

class Robot;  // Forward class declaration to resolve the circular dependency

class TokenManager {
 public:
    explicit TokenManager(
        ::bosdyn::client::Robot* robot,
        ::bosdyn::common::Duration refresh_interval = std::chrono::seconds(3600),
        ::bosdyn::common::Duration initial_retry_interval = std::chrono::seconds(1));

    ~TokenManager();

    // Return true if the thread is running.
    bool IsAlive() const;

    void Stop();

    void SetTokenRefreshErrorCallback(
        std::function<ErrorCallbackResult(const ::bosdyn::common::Status&)> callback);

 private:
    // Refreshes the user token as needed.
    void Update();

    ::bosdyn::client::Robot* m_robot;

    // Refresh token thread.
    std::thread m_refresh_thread;

    // Mutex to protect the token refresh callback and thread liveness boolean.
    std::mutex m_refresh_mutex;

    // Object used to coordinate the lifecycle and timing of the periodic thread.
    std::unique_ptr<PeriodicThreadHelper> m_periodic_thread_helper;

    // Boolean indicating it the thread is still running or if it has returned.
    bool m_refresh_thread_is_alive;

    // Duration between token refreshes. This defaults to 1 hour, but can be updated by
    // the value passed to the constructor.
    ::bosdyn::common::Duration m_user_token_refresh_interval;

    // Initial duration between retries when a token refresh fails, where successive retries
    // back off by a factor of two up to m_user_token_refresh_interval. This defaults to 1 second,
    // but can be updated by the value passed to the constructor.
    ::bosdyn::common::Duration m_user_token_initial_retry_interval;

    // Callback to be invoked when persistent token refresh operations have failed and the
    // the token is expired.
    std::function<ErrorCallbackResult(const ::bosdyn::common::Status&)>
        m_token_refresh_error_callback;
};

}  // namespace client

}  // namespace bosdyn
