/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <google/protobuf/duration.pb.h>
#include <google/protobuf/timestamp.pb.h>
#include <cinttypes>

#include <chrono>
#include <functional>
#include <string>


#ifdef _WIN32
#    define CONVERT_DURATION_FOR_GRPC(tp) std::chrono::duration_cast<std::chrono::microseconds>(tp)
#else
#    define CONVERT_DURATION_FOR_GRPC(tp) tp
#endif

namespace bosdyn {

namespace common {

/// Return nanoseconds since the UNIX epoch as chrono::duration value.
std::chrono::nanoseconds NsecSinceEpoch();

/// Return seconds since the UNIX epoch as chrono::duration value.
std::chrono::seconds SecSinceEpoch();

/// Return a timestamp as nanoseconds since the UNIX epoch.
int64_t NowNsec();
/// This version uses the current wall-clock time, even during a simulation where NowNsec()
///  uses simulation time.
int64_t NowNsecWall();

/// Return a timestamp as nanoseconds since the UNIX epoch.
::google::protobuf::Timestamp NowTimestamp();

using ClockFn = std::function<std::chrono::nanoseconds()>;

/// Set a clock function which overrides default functionality of NowNsec.
void SetClock(const ClockFn& fn);

/// Restore default implementation of NowNsec.
void RestoreDefaultClock();

/// Set a Timestamp proto from nanoseconds since the UNIX epoch.
void SetTimestamp(int64_t nsec, ::google::protobuf::Timestamp* timestamp);

/// Set a Timestamp proto from nanoseconds since the UNIX epoch.
void SetTimestamp(const std::chrono::nanoseconds& nsec, ::google::protobuf::Timestamp* timestamp);

/// Return a protobuf timestamp for a chrono nanoseconds time.
::google::protobuf::Timestamp Timestamp(const std::chrono::nanoseconds& nsec);

/// Given a Timestamp proto, return from nanoseconds since the UNIX epoch as an int64_t.
int64_t TimestampToNsec(const ::google::protobuf::Timestamp& timestamp);

/// Given a Timestamp proto, return duration since the UNIX epoch as std::chrono::nanoseconds.
std::chrono::nanoseconds TimestampToNanosecondsSinceEpoch(
    const ::google::protobuf::Timestamp& timestamp);

/// Given a Timestamp proto, return as string in RFC 3339 format
std::string TimestampToDateString(const ::google::protobuf::Timestamp& timestamp);

/// Given nanoseconds since the UNIX epoch, return as string in RFC 3339 format
std::string NsecToDateString(const std::chrono::nanoseconds& nsec);

/// Set a Duration proto from nanoseconds since the UNIX epoch.
void SetDuration(int64_t nsec, ::google::protobuf::Duration* duration);

/// Set a Duration proto from nanoseconds since the timestamp.
void SetDurationSinceTimestamp(const ::google::protobuf::Timestamp& timestamp,
                               ::google::protobuf::Duration* duration);

/// Convert a duration to nanoseconds..
int64_t DurationToNsec(const ::google::protobuf::Duration& duration);

/// Convert a duration to seconds.
double DurationToSec(const ::google::protobuf::Duration& duration);

// Convert seconds to a duration
::google::protobuf::Duration SecToDuration(double seconds);

/// Returns whether d1 < d2.
bool DurationIsLessThan(const ::google::protobuf::Duration& d1,
                        const ::google::protobuf::Duration& d2);

/// Returns whether d1 <= d2.
bool DurationIsLE(const ::google::protobuf::Duration& d1, const ::google::protobuf::Duration& d2);

inline int64_t SecToNsec(double sec) { return static_cast<int64_t>(sec * 1e9); }

inline double NsecToSec(int64_t nsec) { return static_cast<double>(nsec) / 1.0e9; }

// Duration representing nanoseconds is used as the default timeout/duration type in the C++ SDK.
typedef std::chrono::nanoseconds Duration;

// TimePoint used as the default system_clock time_point type in the C++ SDK.
typedef std::chrono::time_point<std::chrono::system_clock, ::bosdyn::common::Duration> TimePoint;

inline ::bosdyn::common::Duration ProtobufDurationToDuration(
    const ::google::protobuf::Duration& duration) {
    return ::bosdyn::common::Duration(DurationToNsec(duration));
}

inline void DurationToProtobufDuration(::bosdyn::common::Duration duration,
                                       ::google::protobuf::Duration* proto_duration) {
    return SetDuration(duration.count(), proto_duration);
}

/// A helper class for limiting how often a job is done.
class RateLimiter {
 public:
    /// |delta_time| is the minimum time between calls to CheckRateLimit() for which that
    ///  function will return true.
    explicit RateLimiter(const std::chrono::nanoseconds& delta_time)
        : m_delta_time(delta_time), m_last_time(0) {}

    /// Returns true unless |now_nsec| is less than |delta_time| since this last returned true.
    /// This will return true the first time it is called.
    bool CheckRateLimit(int64_t now_nsec) {
        const std::chrono::nanoseconds now(now_nsec);
        if (now - m_last_time < m_delta_time) {
            return false;
        }
        m_last_time = now;
        return true;
    }
    bool CheckRateLimit() { return CheckRateLimit(NowNsec()); }
    bool CheckRateLimit(const std::chrono::time_point<std::chrono::steady_clock>& now) {
        return CheckRateLimit(
            std::chrono::duration_cast<::std::chrono::nanoseconds>(now.time_since_epoch()).count());
    }

 private:
    const std::chrono::nanoseconds m_delta_time;
    std::chrono::nanoseconds m_last_time;
};

}  // namespace common

}  // namespace bosdyn
