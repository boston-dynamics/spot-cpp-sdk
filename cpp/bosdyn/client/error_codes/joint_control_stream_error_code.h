/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <system_error>

// Need to be more specific or removed when streaming client is changed to use general form as we
// did in other clients.
enum class JointControlStreamErrorCode {
    // Success
    Success = 0,
    // Getting request_writer failed
    RequestWriterFailed = 1,
    // Getting response reader failed
    ResponseReaderFailed = 2,
    // Streaming for joint control failed
    StreamingFailed = 3,
};

namespace std {
template <>
struct is_error_code_enum<JointControlStreamErrorCode> : true_type {};
}  // namespace std

std::error_code make_error_code(JointControlStreamErrorCode);