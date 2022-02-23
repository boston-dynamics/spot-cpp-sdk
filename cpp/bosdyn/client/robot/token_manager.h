/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include "robot.h"

namespace bosdyn {

namespace client {

class Robot;  // Forward class declaration to resolve the circular dependency

class TokenManager {
 public:
    explicit TokenManager(
        ::bosdyn::client::Robot* robot, ::bosdyn::common::Duration refresh_interval = std::chrono::seconds(3600));

    ~TokenManager();

    void Stop();

 private:
    // Refreshes the user token as needed.
    void Update();

    // Wait for the refresh interval and coordinate with the destructor on the condition variable.
    bool WaitForInterval();

    ::bosdyn::client::Robot* m_robot;

    // Refresh token thread.
    std::thread m_refresh_thread;

    // Lock for the condition variable
    std::mutex m_refresh_mutex;

    // Condition Variable for checking if the thread should still be sleeping or be stopped.
    std::condition_variable m_cv;

    // Boolean indicating it the thread is still running or if it has returned.
    bool m_refresh_thread_is_alive;

    // Duration between token refreshes. This defaults to 1 hour, but can be updated by
    // the value passed to the constructor.
    ::bosdyn::common::Duration m_user_token_refresh_interval;
};

}  // namespace client

}  // namespace bosdyn
