/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

namespace bosdyn {
namespace client {

enum class ErrorCallbackResult {
    kDefaultAction,
    kRetryImmediately,
    kRetryWithExponentialBackOff,
    kResumeNormalOperation,
    kAbort,
};

}  // namespace client
}  // namespace bosdyn
