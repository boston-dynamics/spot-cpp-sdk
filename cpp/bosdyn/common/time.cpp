/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


/// These are utilities for use in writing services.
#include "time.h"

#include <google/protobuf/util/time_util.h>

#include <chrono>
#include <string>

#include "bosdyn/common/numbers.h"

namespace bosdyn {

namespace common {

namespace {

::std::chrono::nanoseconds _default_clock_fn() {
    return ::std::chrono::duration_cast<::std::chrono::nanoseconds>(
        ::std::chrono::system_clock::now().time_since_epoch());
}

ClockFn _clock_fn = _default_clock_fn;

}  // namespace

using TimeUtil = google::protobuf::util::TimeUtil;

/// Set a clock function which overrides default functionality of NowNsec.
void SetClock(const ClockFn& fn) { _clock_fn = fn; }

void RestoreDefaultClock() { _clock_fn = _default_clock_fn; }

std::chrono::nanoseconds NsecSinceEpoch() { return _clock_fn(); }

std::chrono::seconds SecSinceEpoch() {
    return ::std::chrono::duration_cast<::std::chrono::seconds>(NsecSinceEpoch());
}

int64_t NowNsec() { return NsecSinceEpoch().count(); }

int64_t NowNsecWall() { return _default_clock_fn().count(); }

::google::protobuf::Timestamp NowTimestamp() { return Timestamp(NsecSinceEpoch()); }

::google::protobuf::Timestamp Timestamp(const std::chrono::nanoseconds& nanos) {
    return TimeUtil::NanosecondsToTimestamp(nanos.count());
}

void SetTimestamp(int64_t nsec, ::google::protobuf::Timestamp* timestamp) {
    const int64_t seconds = nsec / kBillion;
    const int64_t nsec_remainder = (nsec - kBillion * seconds);
    timestamp->set_seconds(seconds);
    timestamp->set_nanos(nsec_remainder);
}

void SetTimestamp(const std::chrono::nanoseconds& nsec, ::google::protobuf::Timestamp* timestamp) {
    SetTimestamp(nsec.count(), timestamp);
}

int64_t TimestampToNsec(const ::google::protobuf::Timestamp& timestamp) {
    return TimeUtil::TimestampToNanoseconds(timestamp);
}

std::chrono::nanoseconds TimestampToNanosecondsSinceEpoch(
    const ::google::protobuf::Timestamp& timestamp) {
    return std::chrono::nanoseconds(TimestampToNsec(timestamp));
}

std::string TimestampToDateString(const ::google::protobuf::Timestamp& timestamp) {
    return TimeUtil::ToString(timestamp);
}

std::string NsecToDateString(const std::chrono::nanoseconds& nsec) {
    return TimestampToDateString(Timestamp(nsec));
}

void SetDuration(int64_t nsec, ::google::protobuf::Duration* duration) {
    const int64_t seconds = nsec / kBillion;
    const int64_t nsec_remainder = (nsec - kBillion * seconds);
    duration->set_seconds(seconds);
    duration->set_nanos(nsec_remainder);
}

void SetDurationSinceTimestamp(const ::google::protobuf::Timestamp& timestamp,
                               ::google::protobuf::Duration* duration) {
    int64_t delta_nsec = NowNsec() - TimestampToNsec(timestamp);
    SetDuration(delta_nsec, duration);
}

int64_t DurationToNsec(const ::google::protobuf::Duration& duration) {
    const int64_t seconds = duration.seconds();
    return (seconds * kBillion) + duration.nanos();
}

double DurationToSec(const ::google::protobuf::Duration& duration) {
    const int64_t seconds = duration.seconds();
    return seconds + static_cast<double>(duration.nanos()) / kBillion;
}

::google::protobuf::Duration SecToDuration(double seconds) {
    return google::protobuf::util::TimeUtil::NanosecondsToDuration(seconds * 1e9);
}


bool DurationIsLessThan(const ::google::protobuf::Duration& d1,
                        const ::google::protobuf::Duration& d2) {
    if (d1.seconds() < d2.seconds()) {
        return true;
    }
    if (d1.seconds() > d2.seconds()) {
        return false;
    }
    return d1.nanos() < d2.nanos();
}

bool DurationIsLE(const ::google::protobuf::Duration& d1, const ::google::protobuf::Duration& d2) {
    if (d1.seconds() < d2.seconds()) {
        return true;
    }
    if (d1.seconds() > d2.seconds()) {
        return false;
    }
    return d1.nanos() <= d2.nanos();
}

}  // namespace common

}  // namespace bosdyn
