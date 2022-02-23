/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#if defined(__GNUC__) || defined(__clang__)
#    define BOSDYN_UNUSED __attribute__((unused))
#else
#    define BOSDYN_UNUSED
#endif

#if defined(__GNUC__) && (__GNUC__ > 2) || ((__GNUC__ == 2) && (__GNUC_MINOR__ > 4)) || \
    defined(__clang__)
#    define BOSDYN_CHECK_PRINTF_FORMAT(X) __attribute__((format(printf, (X), 1 + (X))))
#else
// No printf-style format checking for non-GCC/clang compilers
#    define BOSDYN_CHECK_PRINTF_FORMAT(X)
#endif

#if defined(__GNUC__) && (__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ > 2)) || \
    defined(__clang__)
#    define BOSDYN_NONNULL(...) __attribute__((nonnull __VA_ARGS__))
#else
// No nullptr checking for non-GCC/clang compilers
#    define BOSDYN_NONNULL(...)
#endif
