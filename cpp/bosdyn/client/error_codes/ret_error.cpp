/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "ret_error.h"

#include <algorithm>
#include <cstdio>
#include <string>

namespace bosdyn {

namespace client {

namespace {

void _default_log(const char* domain_str, const char* msg) {
    fprintf(stderr, "%s %s\n", domain_str, msg);
}

RetError::LogFunction s_log_fn = _default_log;

// Helper function moves a string and reverses it.
// The ranges [dst, dst+len) and [src, src+len) may overlap.
// Simple implementation is:
//   std::reverse(src, src + len);
//   memmove(dst, src, len);
void reverseAndMove(char* dst, char* src, int len) {
    // Determine where exactly the overlap is between the
    // source and destination regions as well as the start
    // of the non-overlapping portion.
    int overlap_bytes = 0;
    char* overlap = src;
    char* nonoverlap_src = src;
    char* nonoverlap_dst = dst;
    if (dst < src && dst + len >= src) {
        overlap_bytes = static_cast<int>((dst + len) - src);
        overlap = src;
        nonoverlap_src = src + overlap_bytes;
        nonoverlap_dst = dst;
    } else if (dst >= src && src + len > dst) {
        overlap_bytes = static_cast<int>((src + len) - dst);
        overlap = dst;
        nonoverlap_src = src;
        nonoverlap_dst = dst + overlap_bytes;
    } else {
        // No overlap, the std::reverse() call will be a noop.
    }
    int nonoverlap_bytes = len - overlap_bytes;

    // Simply reverse the overlap region.
    std::reverse(overlap, overlap + overlap_bytes);

    // Move the bytes that are not in the overlap region, in reverse.
    for (int i = 0; i < nonoverlap_bytes; i++) {
        nonoverlap_dst[i] = nonoverlap_src[(nonoverlap_bytes - 1) - i];
    }
}

}  // namespace

void RetError::SetLogFunction(LogFunction fn) { s_log_fn = fn; }

void RetError::CopyFrom(const RetError& err) {
    m_code = err.Code();
    strncpy(m_buffer, err.m_buffer, m_buffer_size);
}

void RetError::Reset() {
    m_code = std::error_code();
    m_buffer[0] = 0;
    m_msg_len = 0;
}

bool RetError::SetErrorf(const std::error_code& code, const char* msg_fmt, ...) {
    va_list ap;
    va_start(ap, msg_fmt);
    SetErrorv(code, msg_fmt, ap);
    va_end(ap);
    return false;
}
bool RetError::AddContextf(const char* context_fmt, ...) {
    if (IsError()) {
        va_list ap;
        va_start(ap, context_fmt);
        AddContextv(context_fmt, ap);
        va_end(ap);
    }
    return false;
}

bool RetError::SetErrorLen(const std::error_code& code, int len, const char* msg) {
    m_code = code;

    len = std::min(len, m_buffer_size - 1);
    if (len < 0) {
        return false;
    }

    memcpy(m_buffer, msg, len);

    m_msg_len = len;
    m_buffer[m_msg_len] = '\0';

    return false;
}

bool RetError::SetErrorv(const std::error_code& code, const char* msg_fmt, va_list args) {
    m_code = code;

    const int bytes = vsnprintf(m_buffer, m_buffer_size, msg_fmt, args);
    if (bytes < 0) {
        // On windows, if the output string doesn't fit, -1 is returned.
        m_msg_len = m_buffer_size;
    } else if (bytes > m_buffer_size) {
        // On POSIX, vsnprintf() returns the size that would have been printed if not truncated.
        m_msg_len = m_buffer_size;
    } else {
        m_msg_len = bytes;
    }
    return false;
}

bool RetError::SetErrorFromErrno(const char* msg) {
    return SetErrorLen(std::error_code(errno, std::system_category()),
                       static_cast<int>(strlen(msg)), msg);
}
bool RetError::SetErrorFromErrnov(const char* msg_fmt, va_list args) {
    return SetErrorv(std::error_code(errno, std::system_category()), msg_fmt, args);
}
bool RetError::SetErrorFromErrnof(const char* msg_fmt, ...) {
    va_list ap;
    va_start(ap, msg_fmt);
    SetErrorv(std::error_code(errno, std::system_category()), msg_fmt, ap);
    va_end(ap);
    return false;
}

int RetError::CacheMsg() {
    const int prev_len = m_msg_len;
    reverseAndMove(m_buffer + (m_buffer_size - prev_len), m_buffer, prev_len);
    return prev_len;
}

void RetError::UnCacheMsg(int cached_len, int bytes_added) {
    const int MIN_SIZE = 4;  // room for ":\n " and '\0'
    if (m_buffer_size < MIN_SIZE) {
        m_msg_len = bytes_added;  // Throw-out the old message.
        return;
    }

    // Always add the ": " separator, even if it's just at the end.
    int new_bytes = std::min(bytes_added, m_buffer_size - MIN_SIZE);
    m_buffer[new_bytes++] = ':';
    m_buffer[new_bytes++] = '\n';
    m_buffer[new_bytes++] = ' ';

    // Put the existing message back in the right order.
    // Note that it may have been truncated and prev_len is reduced.
    const int restore_bytes = std::min(cached_len, (m_buffer_size - 1) - new_bytes);

    char* truncated_prev_msg = m_buffer + (m_buffer_size - restore_bytes);
    reverseAndMove(m_buffer + new_bytes, truncated_prev_msg, restore_bytes);

    m_msg_len = new_bytes + restore_bytes;
    m_buffer[m_msg_len] = '\0';
}

bool RetError::AddContextLen(int len, const char* msg) {
    if (!IsError()) {
        return false;
    }
    len = std::min(len, m_buffer_size - 1);
    if (len < 0) {
        return false;
    }

    // Put the existing message at the end of the buffer in reverse.
    const int prev_len = CacheMsg();

    // Write the new message to the front of the buffer.
    memcpy(m_buffer, msg, len);

    // Restore previous message the best we can.
    UnCacheMsg(prev_len, len);

    return false;
}

bool RetError::AddContextv(const char* context_fmt, va_list args) {
    if (!IsError()) {
        return false;
    }

    // Put the existing message at the end of the buffer in reverse.
    const int prev_len = CacheMsg();

    const int bytes = vsnprintf(m_buffer, m_buffer_size, context_fmt, args);
    // On windows, if the output string doesn't fit, -1 is returned.
    const int new_bytes = (bytes >= 0 ? bytes : m_buffer_size);

    // Restore previous message the best we can.
    UnCacheMsg(prev_len, new_bytes);

    return false;
}

}  // namespace client

}  // namespace bosdyn
