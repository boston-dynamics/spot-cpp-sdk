/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "token_manager.h"


namespace bosdyn {

namespace client {

TokenManager::TokenManager(Robot* robot, ::bosdyn::common::Duration refresh_interval,
                           ::bosdyn::common::Duration initial_retry_interval)
    : m_robot(robot),
      m_periodic_thread_helper(std::make_unique<PeriodicThreadHelper>()),
      m_refresh_thread_is_alive(true),
      m_user_token_refresh_interval(refresh_interval),
      m_user_token_initial_retry_interval(initial_retry_interval) {
    m_refresh_thread = std::thread(&TokenManager::Update, this);
}


TokenManager::~TokenManager() { Stop(); }

bool TokenManager::IsAlive() const { return m_refresh_thread_is_alive; }

void TokenManager::Stop() {
    m_periodic_thread_helper->Stop();
    if (m_refresh_thread.joinable()) {
        m_refresh_thread.join();
    }
}

void TokenManager::Update() {
    ::bosdyn::common::Duration retry_interval = m_user_token_initial_retry_interval;
    ::bosdyn::common::Duration wait_interval = m_user_token_refresh_interval;
    while (m_periodic_thread_helper->WaitForInterval(wait_interval)) {
        ::bosdyn::common::Status status = m_robot->AuthenticateWithToken(m_robot->GetUserToken());
        if (!status) {
            fprintf(stderr, "AuthenticateWithToken failed: '%s'\n", status.DebugString().c_str());
            ErrorCallbackResult result = ErrorCallbackResult::kRetryWithExponentialBackOff;
            if (status.code().value() ==
                ::bosdyn::api::GetAuthTokenResponse::STATUS_INVALID_TOKEN) {
                std::function<ErrorCallbackResult(const ::bosdyn::common::Status&)> callback;
                {
                    // Lock the mutex to safely access the callback.
                    std::lock_guard<std::mutex> lock(m_refresh_mutex);
                    callback = m_token_refresh_error_callback;
                }
                if (callback) {
                    // Notify the callback that the token has expired.
                    try {
                        result = callback(status);
                    } catch (const std::exception& e) {
                        fprintf(stderr, "Exception in token refresh error callback: %s\n",
                                e.what());
                    }
                }
            }
            if (result == ErrorCallbackResult::kRetryWithExponentialBackOff ||
                result == ErrorCallbackResult::kDefaultAction) {
                // Exponentially increase the retry interval.
                wait_interval = retry_interval;
                fprintf(stderr, "Retrying in %f seconds\n", ((double)wait_interval.count()) / 1e9);
                retry_interval = std::min(retry_interval * 2, m_user_token_refresh_interval);
            } else if (result == ErrorCallbackResult::kAbort) {
                fprintf(stderr, "Aborting token manager thread.\n");
                break;
            } else if (result == ErrorCallbackResult::kResumeNormalOperation) {
                // Reset the retry interval to the initial value.
                retry_interval = m_user_token_initial_retry_interval;
                wait_interval = m_user_token_refresh_interval;
            } else if (result == ErrorCallbackResult::kRetryImmediately) {
                // Retry immediately.
                wait_interval = std::chrono::milliseconds(0);
            }
        } else {
            // Reset the retry interval on success.
            retry_interval = m_user_token_initial_retry_interval;
            wait_interval = m_user_token_refresh_interval;
        }
    }
    // Callback result of kAbort will cause the thread to exit; mark it as down.
    {
        std::lock_guard<std::mutex> lock(m_refresh_mutex);
        m_refresh_thread_is_alive = false;
    }
    m_periodic_thread_helper->Stop();
}

void TokenManager::SetTokenRefreshErrorCallback(
    std::function<ErrorCallbackResult(const ::bosdyn::common::Status&)> callback) {
    std::lock_guard<std::mutex> lock(m_refresh_mutex);
    m_token_refresh_error_callback = callback;
}

}  // namespace client

}  // namespace bosdyn
