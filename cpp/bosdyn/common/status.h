/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <string>
#include <system_error>

namespace bosdyn {

namespace common {

class [[nodiscard]] Status {
 public:
    Status() = default;

    ~Status() = default;

    Status(const std::error_code& code, std::string message = "");

    operator bool() const;

    std::error_code code() const { return m_code; }

    // Check if this matches a certain error enum.
    template <typename ENUM>
    bool Is() const {
        static_assert(std::is_error_code_enum<ENUM>::value, "Must check against an error code enum");
        return std::error_code(ENUM{}).category() == m_code.category();
    }

    const std::string& message() const { return m_message; }

    // Extend a ::bosdyn::common::Status with a new message.
    ::bosdyn::common::Status Chain(std::string message) const;

    // Transform a ::bosdyn::common::Status into a new code.
    ::bosdyn::common::Status Chain(std::error_code code, std::string message) const;

    std::string DebugString() const;

    // Used to explicitly ignore any error present to silence nodiscard warnings.
    inline void IgnoreError() const {}

 private:
    std::error_code m_code;
    std::string m_message;
};

#define STATUS_OK_ELSE_RETURN(expr)               \
    do {                                          \
        auto local_status_ = (expr);              \
        if (!local_status_) return local_status_; \
    } while (0);

#define STATUS_OK_ELSE_RETURN_WITH_MSG(expr, msg)              \
    do {                                                       \
        auto local_status_ = (expr);                           \
        if (!local_status_) return local_status_.Chain((msg)); \
    } while (0);

#define STATUS_OK_ELSE_RETURN_WITH_CODE(expr, new_code, msg)               \
    do {                                                                   \
        auto local_status_ = (expr);                                       \
        if (!local_status_) return local_status_.Chain((new_code), (msg)); \
    } while (0);
}  // namespace common

}  // namespace bosdyn
