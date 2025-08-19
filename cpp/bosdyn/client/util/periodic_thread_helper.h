/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <condition_variable>
#include <mutex>

#include "bosdyn/common/time.h"

namespace bosdyn {
namespace client {

class PeriodicThreadHelper {
 public:
    PeriodicThreadHelper() = default;
    virtual ~PeriodicThreadHelper() = default;

    // Indicates that the thread should exit.
    virtual void Stop();

    // Wait for the specified interval. If thread shutdown was requested, return false.
    virtual bool WaitForInterval(::bosdyn::common::Duration interval);

 private:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_should_exit = false;
};

}  // namespace client
}  // namespace bosdyn
