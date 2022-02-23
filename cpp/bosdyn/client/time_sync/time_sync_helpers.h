/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <mutex>
#include <string>
#include <thread>

#include "bosdyn/client/service_client/common_result_types.h"
#include "bosdyn/common/robot_time_converter.h"
#include "bosdyn/client/time_sync/time_sync_client.h"

namespace bosdyn {

namespace client {

// The TimeSyncEndpoint is a wrapper around TimeSyncClient used to establish and maintain timesync
// with a robot. This class manages internal state and can automatically build requests to pass
// through the TimeSyncClient without exposing the details of managing timesync.
// This object is thread-safe.
class TimeSyncEndpoint {
 public:
    explicit TimeSyncEndpoint(TimeSyncClient* client);

    TimeSyncUpdateResultType GetResult();

    bool HasEstablishedTimeSync();

    DurationResultType GetRoundTripTime();

    StringResultType GetClockIdentifier();

    DurationResultType GetClockSkew();

    bool GetNewEstimate();

    bool EstablishTimeSync(int max_samples, bool break_on_success);

    ::bosdyn::common::RobotTimeConverter GetRobotTimeConverter() const;

    google::protobuf::Timestamp RobotTimestampFromLocal(::bosdyn::common::TimePoint local_time);

 private:
    TimeSyncUpdateResultType Update();

    TimeSyncClient* m_client = nullptr;
    std::mutex m_mutex;
    // These member variables should be accessed using the lock.
    ::bosdyn::api::TimeSyncRoundTrip m_locked_previous_round_trip;
    TimeSyncUpdateResultType m_locked_previous_result;
    std::string m_locked_clock_identifier;
};

// The TimeSyncThread class is used to establish and maintain robot timesync asynchronously.
class TimeSyncThread {
 public:
    explicit TimeSyncThread(TimeSyncClient* client, ::bosdyn::common::Duration time_sync_interval =
                            std::chrono::nanoseconds(int64_t(60 * 1e9)))
        : m_time_sync_interval(time_sync_interval),
          m_time_sync_endpoint(client),
          m_thread() {}

    ~TimeSyncThread() { Stop(); }

    // Start the thread if it is not yet running.
    void Start();

    // Shut down the thread if it is running.
    void Stop();

    // Trigger the thread stop sequence.
    bool ShouldExit();

    // Return the TimeSyncEndpoint in use.
    TimeSyncEndpoint* GetEndpoint() { return &m_time_sync_endpoint; }

    // Return true if time sync has been established with the robot.
    bool HasEstablishedTimeSync() { return m_time_sync_endpoint.HasEstablishedTimeSync(); }

    // Wait up to timeout for time sync to be achieved.
    bool WaitForSync(::bosdyn::common::Duration timeout = std::chrono::seconds(3));

    // Get the current estimate for robot clock skew from local time.
    DurationResultType GetRobotClockSkew(
        ::bosdyn::common::Duration time_sync_timeout = std::chrono::seconds(0));

    ::bosdyn::common::RobotTimeConverterResultType GetRobotTimeConverter(
        ::bosdyn::common::Duration timeout = std::chrono::seconds(0));

    TimestampResultType RobotTimestampFromLocal(::bosdyn::common::TimePoint local_time);

 private:
    // Background thread that achieves and maintains timesync with the robot.
    void TimeSyncThreadMethod();

    // Return true if the thread is no longer running. Passed in
    // unique_lock should own m_mutex before calling.
    bool IsStoppedLocked(const std::lock_guard<std::mutex>& lock) const;

    // After achieving time sync, update estimate at this interval.
    ::bosdyn::common::Duration m_time_sync_interval;
    // When the time sync service is not yet ready, poll it at this interval.
    const ::bosdyn::common::Duration kTimeSyncServiceNotReadyInterval = std::chrono::seconds(5);
    // How long to sleep in between stop request checks (10 ms)
    const ::bosdyn::common::Duration kSleepInterval = std::chrono::nanoseconds(10000000);
    // TimeSyncEndpoint object created with the provided TimeSyncClient.
    TimeSyncEndpoint m_time_sync_endpoint;

    std::mutex m_mutex;
    bool m_locked_should_exit = false;
    bool m_locked_thread_stopped = true;
    std::thread m_thread;
};

}  // namespace client

}  // namespace bosdyn
