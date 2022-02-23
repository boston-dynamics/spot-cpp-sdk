/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <cstdint>

namespace bosdyn {

namespace common {

/// A class for wrapping int32 values in a more opaque value, for better type safety.
/// This considers negative values is being invalid.
class Int32Wrapper {
 public:
    explicit Int32Wrapper(int32_t val) : m_val(val) {}
    Int32Wrapper(const Int32Wrapper&) = default;
    Int32Wrapper& operator=(const Int32Wrapper&) = default;
    int32_t Val() const { return m_val; }
    bool IsOk() const { return m_val >= 0; }
    bool operator<(const Int32Wrapper& other) const { return m_val < other.Val(); }

 private:
    int32_t m_val;
};

}  // namespace common

}  // namespace bosdyn
