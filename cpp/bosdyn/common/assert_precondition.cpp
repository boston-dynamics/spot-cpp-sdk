/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "assert_precondition.h"
#include <cstdarg>

namespace bosdyn {

namespace common {

namespace {

std::string formatString(const char* format, va_list args) noexcept {
    std::string reason;

    va_list tmp;
    va_copy(tmp, args);
    int n = vsnprintf(nullptr, 0, format, tmp);
    va_end(tmp);

    if (n >= 0) {
        ++n;  // for the nul-terminating char
        char* buf = new char[n];
        vsnprintf(buf, n, format, args);
        reason.assign(buf);
        delete[] buf;
    }
    return reason;
}

std::string panicString(const char* function, int line, const char* file, const char* cond, const char* reason) {
    std::stringstream stream;
    stream << "Pre-Condition Assert Failed:" << std::endl
           << "in " << function << std::endl
           << "at line " << line << std::endl
           << "in file " << file << std::endl;
    if (cond) {
        stream << "this condition evaluated to false (expected true): " << cond << std::endl;
    }
    stream << "reason: " << reason << std::endl;
    return stream.str();
}

} // namespace

void panic(char const* function, char const* file, int line, const char* cond,
                  const char* format, ...) {
    va_list args;
    va_start(args, format);
    std::string reason(formatString(format, args));
    va_end(args);
    const std::string msg = panicString(function, line, file, cond, reason.c_str());
    fprintf(stderr, "%s\n", msg.c_str());
    std::terminate();
}

}  // namespace common
}  // namespace bosdyn
