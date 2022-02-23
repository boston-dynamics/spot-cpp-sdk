/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include "bosdyn/common/status.h"

namespace bosdyn {

namespace client {


template <typename ResponseType>
struct [[nodiscard]] Result {
    ::bosdyn::common::Status status;
    ResponseType response;

    // Result can be used directly in if statements to check if it was successful.
    operator bool() const { return status; }

    // Conditionally move the response into output, if status == true.
    // The Result should not be used after calling this function, as the status and response
    // may have been moved away.
    ::bosdyn::common::Status&& move_to(ResponseType* output) {
        if (status) *output = std::move(response);
        return std::move(status);
    }

    // Helper for moving out the response.
    // Usage example:
    // auto result = function_that_returns_result();
    // if (result){
    //     auto value = result.move();
    //     ... use value here ...
    // }
    // 
    // The Result should not be used after calling this function, as the response
    // may have been moved away.
    ResponseType&& move() { return std::move(response); }

    // Used to explicitly ignore any error present to silence nodiscard warnings.
    inline void IgnoreError() const {}
};

}  // namespace client

}  // namespace bosdyn
