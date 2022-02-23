/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <cassert>
#include <sstream>
#include <exception>
#include <functional>
#include <stdexcept>
#include <string>

#ifdef BOSDYN_DISABLE_ASSERTS
// Disables all bosdyn assert checks.
#define BOSDYN_ASSERT_PRECONDITION(cond, ...) ((void)0)
#else

/**
 * The SDK uses assertions to check pre-conditions required for specific functions to complete
 * successfully.
 *
 * The BOSDYN_ASSERT_PRECONDITION macro will check a condition, and if it is not met then it will
 * report the failure and terminate the program.
 *
 * If arguments are provided after the condition, then the first argument after condition will be
 * used as a printf-style format string, and the following arguments will be used as arguments to
 * that format string.
 */
#define BOSDYN_ASSERT_PRECONDITION(cond, ...)                                                          \
    (!(cond) ? bosdyn::common::panic(__FUNCTION__, __FILE__, __LINE__, #cond, ##__VA_ARGS__) \
             : (void)0)
#endif


namespace bosdyn {

namespace common {

// Logs a formatted error message with additional debug information, then will terminate the program.
[[noreturn]] void panic(char const* function, char const* file, int line, const char* cond, const char* format, ...);

}  // namespace common

}  // namespace bosdyn
