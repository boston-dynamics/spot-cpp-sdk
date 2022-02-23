/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "time_sync_helpers.h"

#include "bosdyn/common/time.h"
#include "bosdyn/client/error_codes/time_sync_helper_error_code.h"
#include "bosdyn/common/assert_precondition.h"

namespace bosdyn {

namespace client {

// TimeSyncEndpoint methods
TimeSyncEndpoint::TimeSyncEndpoint(TimeSyncClient* client) {
    m_client = client;
    std::lock_guard<std::mutex> lock(m_mutex);
    m_locked_previous_result.status =
        ::bosdyn::common::Status(RobotTimeSyncErrorCode::PreviousTimeSyncUnavailableYet, "Result not yet populated");
}

TimeSyncUpdateResultType TimeSyncEndpoint::GetResult() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_locked_previous_result;
}

bool TimeSyncEndpoint::HasEstablishedTimeSync() {
    TimeSyncUpdateResultType update_result = GetResult();
    return update_result.status &&
           update_result.response.state().status() == ::bosdyn::api::TimeSyncState::STATUS_OK;
}

DurationResultType TimeSyncEndpoint::GetRoundTripTime() {
    DurationResultType result;
    ::bosdyn::api::TimeSyncUpdateResponse response;
    TimeSyncUpdateResultType update_result = GetResult();
    if (!update_result.status) {
        result.status = update_result.status;
    } else {
        result = {::bosdyn::common::Status(SDKErrorCode::Success),
                  std::make_shared<::google::protobuf::Duration>(
                      response.state().best_estimate().round_trip_time())};
    }
    return result;
}

StringResultType TimeSyncEndpoint::GetClockIdentifier() {
    StringResultType result;
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_locked_clock_identifier.empty()) {
        result.status =
            ::bosdyn::common::Status(TimeSyncHelperErrorCode::ClockIdentifierUnset, "Clock identifier cannot be empty.");
    } else {
        result = {::bosdyn::common::Status(SDKErrorCode::Success),
                    std::make_shared<std::string>(m_locked_clock_identifier)};
    }
    return result;
}

DurationResultType TimeSyncEndpoint::GetClockSkew() {
    DurationResultType result;
    ::bosdyn::api::TimeSyncUpdateResponse response;
    TimeSyncUpdateResultType update_result = GetResult();
    if (!update_result.status) {
        result.status = update_result.status;
        return result;
    }
    if (update_result.response.state().status() != ::bosdyn::api::TimeSyncState::STATUS_OK) {
        result.status = ::bosdyn::common::Status(update_result.response.state().status(),
                               "GetClockSkew: Clock synchronization not yet achieved.");
        return result;
    }
    result = {::bosdyn::common::Status(SDKErrorCode::Success),
              std::make_shared<google::protobuf::Duration>(
                  update_result.response.state().best_estimate().clock_skew())};
    return result;
}

bool TimeSyncEndpoint::GetNewEstimate() {
    TimeSyncUpdateResultType update_result = Update();
    if (!update_result.status) {
        std::cerr << "GetNewEstimate: Update failed - " << update_result.response.DebugString()
                  << std::endl;
        return false;
    }
    ::google::protobuf::Timestamp rx_time = ::bosdyn::common::NowTimestamp();

    // Record the timing info for this gRPC call to pass to the next update.
    ::bosdyn::api::TimeSyncRoundTrip round_trip;
    round_trip.mutable_client_tx()->CopyFrom(
        update_result.response.header().request_header().request_timestamp());
    round_trip.mutable_server_rx()->CopyFrom(
        update_result.response.header().request_received_timestamp());
    round_trip.mutable_server_tx()->CopyFrom(update_result.response.header().response_timestamp());
    round_trip.mutable_client_rx()->CopyFrom(rx_time);

    // Update state.
    std::lock_guard<std::mutex> lock(m_mutex);
    m_locked_previous_round_trip = round_trip;
    m_locked_previous_result = update_result;
    m_locked_clock_identifier = update_result.response.clock_identifier();

    return true;
}

bool TimeSyncEndpoint::EstablishTimeSync(int max_samples, bool break_on_success) {
    for (int counter = 0; counter < max_samples; counter++) {
        if (break_on_success && HasEstablishedTimeSync()) {
            return true;
        }
        GetNewEstimate();
    }
    return HasEstablishedTimeSync();
}

::bosdyn::common::RobotTimeConverter TimeSyncEndpoint::GetRobotTimeConverter() const {
    return ::bosdyn::common::RobotTimeConverter(std::chrono::nanoseconds(
        ::bosdyn::common::DurationToNsec(
            m_locked_previous_result.response.state().best_estimate().clock_skew())));
}

google::protobuf::Timestamp TimeSyncEndpoint::RobotTimestampFromLocal(
    ::bosdyn::common::TimePoint local_time) {
    ::bosdyn::common::RobotTimeConverter robot_time_converter = GetRobotTimeConverter();
    return robot_time_converter.RobotTimestampFromLocal(local_time);
}

TimeSyncUpdateResultType TimeSyncEndpoint::Update() {
    ::bosdyn::api::TimeSyncUpdateRequest request;
    // If clock_identifier not set, use empty clock_identifier.
    // Only add a round trip to a request that contains a clock identifier, otherwise the
    // sever will respond with an invalid request error.  Responses with errors may not
    // contain a clock identifier. This may happen, for example, if the service was not
    // yet ready at the time of the request.
    StringResultType clock_id_result = GetClockIdentifier();
    if (clock_id_result.status) {
        request.set_clock_identifier(*clock_id_result.response);
        request.mutable_previous_round_trip()->CopyFrom(m_locked_previous_round_trip);
    }

    return m_client->TimeSyncUpdate(request);
}

// TimeSyncThread Methods
void TimeSyncThread::Start() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!IsStoppedLocked(lock)) {
        return;
    }
    m_locked_should_exit = false;
    m_locked_thread_stopped = false;
    m_thread = std::thread(&TimeSyncThread::TimeSyncThreadMethod, this);
}

void TimeSyncThread::Stop() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (IsStoppedLocked(lock)) {
            return;
        }
        m_locked_should_exit = true;
    }
    m_thread.join();
}

bool TimeSyncThread::IsStoppedLocked(const std::lock_guard<std::mutex>& lock) const {
    return m_locked_thread_stopped;
}

bool TimeSyncThread::ShouldExit() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_locked_should_exit;
}

bool TimeSyncThread::WaitForSync(::bosdyn::common::Duration timeout) {
    if (HasEstablishedTimeSync()) {
        return true;
    }
    auto end_time = std::chrono::system_clock::now() + timeout;
    while (std::chrono::system_clock::now() < end_time) {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (IsStoppedLocked(lock)) {
                return false;
            }
        }
        if (HasEstablishedTimeSync()) {
            return true;
        }
        std::this_thread::sleep_for(kSleepInterval);
    }
    return false;
}

DurationResultType TimeSyncThread::GetRobotClockSkew(::bosdyn::common::Duration time_sync_timeout) {
    DurationResultType result;
    if (!WaitForSync(time_sync_timeout)) {
        result.status = ::bosdyn::common::Status(EstablishTimeSyncErrorCode::UnableToEstablishTimeSync,
                               "GetRobotClockSkew: Timed out waiting for time synchronization.");
        return result;
    }
    result = {::bosdyn::common::Status(SDKErrorCode::Success),
              std::make_shared<google::protobuf::Duration>(
                  m_time_sync_endpoint.GetResult().response.state().best_estimate().clock_skew())};
    return result;
}

::bosdyn::common::RobotTimeConverterResultType TimeSyncThread::GetRobotTimeConverter(
    ::bosdyn::common::Duration timeout) {
    ::bosdyn::common::RobotTimeConverterResultType result;
    if (!WaitForSync(timeout)) {
        result.status = ::bosdyn::common::Status(EstablishTimeSyncErrorCode::UnableToEstablishTimeSync,
                               "GetRobotTimeConverter: Failed to establish timesync.");
        return result;
    }
    ::bosdyn::common::RobotTimeConverter robot_time_converter =
        m_time_sync_endpoint.GetRobotTimeConverter();
    result = {::bosdyn::common::Status(SDKErrorCode::Success),
              std::make_shared<::bosdyn::common::RobotTimeConverter>(robot_time_converter)};
    return result;
}

TimestampResultType TimeSyncThread::RobotTimestampFromLocal(::bosdyn::common::TimePoint local_time) {
    TimestampResultType result;
    ::bosdyn::common::RobotTimeConverterResultType get_robot_time_converter_result =
        GetRobotTimeConverter();
    if (!get_robot_time_converter_result.status) {
        result.status = result.status.Chain(
            "RobotTimestampFromLocal: Failed to get robot time converter.");
        return result;
    }
    result = {::bosdyn::common::Status(SDKErrorCode::Success),
              std::make_shared<google::protobuf::Timestamp>(
                  get_robot_time_converter_result.response->RobotTimestampFromLocal(local_time))};
    return result;
}

void TimeSyncThread::TimeSyncThreadMethod() {
    while (true) {
        // If exit requested, break from loop.
        if (ShouldExit()) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_locked_thread_stopped = true;
            return;
        }

        TimeSyncUpdateResultType result = m_time_sync_endpoint.GetResult();
        auto sleep_nanos = std::chrono::nanoseconds(0);
        // If time sync service not running, wait for interval before polling again.
        if (result.status && result.response.state().status() ==
                                 ::bosdyn::api::TimeSyncState::STATUS_SERVICE_NOT_READY) {
            sleep_nanos = kTimeSyncServiceNotReadyInterval;
        }
        // Else if time sync already established, wait for interval before polling again.
        else if (result.status &&
                 result.response.state().status() == ::bosdyn::api::TimeSyncState::STATUS_OK) {
            sleep_nanos = m_time_sync_interval;
        }  // Else if time sync not yet established, no delay.

        // Wait selected time, break if stop requested
        auto end_time = std::chrono::system_clock::now() + sleep_nanos;
        while (std::chrono::system_clock::now() < end_time) {
            std::this_thread::sleep_for(kSleepInterval);
            if (ShouldExit()) {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_locked_thread_stopped = true;
                return;
            }
        }

        // Make RPC call to update time sync estimate.
        if (!m_time_sync_endpoint.GetNewEstimate()) {
            std::cerr << "TimeSyncThreadMethod failed to get new estimate";
        }
    }
}

}  // namespace client

}  // namespace bosdyn
