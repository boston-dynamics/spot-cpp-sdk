/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "token_manager.h"


namespace bosdyn {

namespace client {

TokenManager::TokenManager(Robot* robot, ::bosdyn::common::Duration refresh_interval) :
    m_robot(robot),
    m_refresh_thread_is_alive(true),
    m_user_token_refresh_interval(refresh_interval){
    m_refresh_thread = std::thread(&TokenManager::Update, this);
}

TokenManager::~TokenManager() {
    Stop();
}

void TokenManager::Stop() {
    {
        std::lock_guard<std::mutex> lock(this->m_refresh_mutex);
        m_refresh_thread_is_alive = false;
    }
    m_cv.notify_one();
    m_refresh_thread.join();
}

bool TokenManager::WaitForInterval() {
    std::unique_lock<std::mutex> lock(m_refresh_mutex);
    if (!m_refresh_thread_is_alive) return false;
    return !m_cv.wait_for(lock, m_user_token_refresh_interval,
                          [this]() { return !m_refresh_thread_is_alive; });
}

void TokenManager::Update() {
    while (WaitForInterval()) {
        ::bosdyn::common::Status status = m_robot->AuthenticateWithToken(m_robot->GetUserToken());
        if (!status) {
            fprintf(stderr, "AuthenticateWithToken failed: '%s'\n",
                    status.DebugString().c_str());
        }
    }
}

}  // namespace client

}  // namespace bosdyn
