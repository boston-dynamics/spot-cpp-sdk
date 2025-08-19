/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/util/periodic_thread_helper.h"

namespace bosdyn {
namespace client {

void PeriodicThreadHelper::Stop() {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_should_exit = true;
    lock.unlock();
    m_cv.notify_all();
}

bool PeriodicThreadHelper::WaitForInterval(::bosdyn::common::Duration interval) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_should_exit) return false;
    return !m_cv.wait_for(lock, interval, [this]() { return m_should_exit; });
}

}  // namespace client
}  // namespace bosdyn
