/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <cerrno>
#include <cstdarg>
#include <cstddef>
#include <cstring>
#include <functional>
#include <string>
#include <system_error>

#include "bosdyn/common/compiler_definitions.h"

namespace bosdyn {

namespace client {

/// A wrapper for std::error_code which also provides a way to include a customized error message
///  while not requiring allocation of memory on the heap.
class RetError {
 public:
    /// Set log function to handle cases where static methods are called with a null RetError arg.
    /// If not set, defaults to fprintf(stderr, "%s %s\n", domain_str, const char* msg).
    typedef void (*LogFunction)(const char* domain_str, const char* msg);
    static void SetLogFunction(LogFunction fn);

    /// Has an error been reported via this object?
    bool IsError() const { return static_cast<bool>(Code()); }

    const std::error_code& Code() const { return m_code; }

    /// Human-readable explanation for error, suitable for printing to user
    const char* ErrMsg() const { return IsError() ? m_buffer : "(no error)"; }

    // Functions to report an error.  These *always* return false.
    bool SetError(const std::error_code& code, const char* msg) {
        return SetErrorLen(code, static_cast<int>(strlen(msg)), msg);
    }
    bool SetErrorv(const std::error_code& code, const char* msg_fmt, va_list args);
    bool SetErrorf(const std::error_code& code, const char* msg_fmt, ...)
        BOSDYN_CHECK_PRINTF_FORMAT(3);
    bool SetErrorFromErrno(const char* msg);
    bool SetErrorFromErrnov(const char* msg_fmt, va_list args);
    bool SetErrorFromErrnof(const char* msg_fmt, ...) BOSDYN_CHECK_PRINTF_FORMAT(2);

    /// Add text at the start of ErrMsg() to describe what was being done when the error occurred.
    ///
    /// @note If err.errMsg() is "stubbed toe", then
    ///
    /// @code
    ///   err.addContext("while kicking the wall")
    /// @endcode
    /// transforms err.errMsg() to "while kicking the wall:\n stubbed toe".
    /// Always returns false, for use in error-reporting in the caller.
    bool AddContext(const char* context) {
        if (IsError() && context) {
            AddContextLen(static_cast<int>(strlen(context)), context);
        }
        return false;
    }
    /// Always returns false, for use in error-reporting in the caller.
    bool AddContextf(const char* context_fmt, ...) BOSDYN_CHECK_PRINTF_FORMAT(2);
    /// Always returns false, for use in error-reporting in the caller.
    bool AddContextv(const char* context_fmt, va_list args);

    /// Remove current error codes as if this is a 'new' RetError object.
    void Reset();

    void CopyFrom(const RetError& err);

 protected:
    RetError(char* buffer, int buff_size) : m_buffer(buffer), m_buffer_size(buff_size) {}

    std::error_code m_code;
    char* m_buffer;
    int m_buffer_size = 0;
    int m_msg_len = 0;  // Not including null terminator.

 private:
    int CacheMsg();
    void UnCacheMsg(int cached_len, int bytes_added);
    bool AddContextLen(int len, const char* context);
    bool SetErrorLen(const std::error_code& code, int len, const char* msg);
};

/// Version of RetError that automatically allocates message storage on the
///   stack.
template <int sz>
class RetErrorBuf : public RetError {
 public:
    RetErrorBuf() : RetError(m_buff, sizeof(m_buff)) {}

 private:
    char m_buff[sz] = {0};
};

/// Adds context information to a RetError object if an error is set when the WithContext
///  object goes out of scope.
class WithContext {
 public:
    WithContext(RetError* error, const char* context) BOSDYN_NONNULL()
        : m_error(error), m_context(context) {}

    ~WithContext() {
        if (m_error->IsError()) {
            m_error->AddContext(m_context);
        }
    }

 private:
    RetError* m_error;
    const char* m_context;
};

/// Calls caller-supplied 'fn' to add context to a RetError object if that RetError object
///  has an error set when the WithContextFn objet goes out of scope.
///
/// Example usage:
/// @code{.cpp}
///  WithContextFn context(
///            &err, [](const WithContextFn::SetContextFn& setctx) { setctx("fn-context"); });
///        err.SetError(bddf::ErrorCode::kDataError, "error");
/// @endcode
class WithContextFn {
 public:
    using SetContextFn = const std::function<void(const char*)>;
    using ContextFn = const std::function<void(const SetContextFn&)>;

    WithContextFn(RetError* error, const ContextFn& fn) BOSDYN_NONNULL()
        : m_error(error), m_fn(fn) {}

    ~WithContextFn() {
        if (m_error->IsError() && m_fn) {
            m_fn([&](const char* context) { m_error->AddContext(context); });
        }
    }

 private:
    RetError* m_error;
    const ContextFn m_fn;
};

/// Calls caller-supplied 'fn' to add context to a RetError object if that RetError object
///  has an error set when the WithContextFn objet goes out of scope.
///
/// Example usage:
/// @code{.cpp}
///        WithContextStrFn context(&err, [](const WithContextStrFn::SetContextFn& setctx) {
///            setctx(std::string("strfn-context"));
///        });
///        err.SetError(bddf::ErrorCode::kDataError, "error");
/// @endcode

class WithContextStrFn {
 public:
    using SetContextFn = const std::function<void(const std::string&)>;
    using ContextFn = const std::function<void(const SetContextFn&)>;

    WithContextStrFn(RetError* error, const ContextFn& fn) BOSDYN_NONNULL()
        : m_error(error), m_fn(fn) {}

    ~WithContextStrFn() {
        if (m_error->IsError() && m_fn) {
            m_fn([&](const std::string& context) { m_error->AddContext(context.c_str()); });
        }
    }

 private:
    RetError* m_error;
    const ContextFn m_fn;
};

}  // namespace client

}  // namespace bosdyn
