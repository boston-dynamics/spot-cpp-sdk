/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <deque>

#include "proto_math.h"

namespace bosdyn {

namespace api {
namespace math {

struct TimedPose {
    int64_t timestamp;
    ::bosdyn::api::SE3Pose pose;
};

// Interpolates between given vectors a and b at the interpolation factor between 0 and 1.
::bosdyn::api::Vec3 Lerp(const ::bosdyn::api::Vec3& a, const ::bosdyn::api::Vec3& b, double factor);
// Interpolates between quaternions v0 and v1 at the given interpolation factor between 0 and 1.
::bosdyn::api::Quaternion Slerp(const ::bosdyn::api::Quaternion& v0, const ::bosdyn::api::Quaternion& v1,
                              double factor);
// Interpolates between poses a and b at the given interpolation factor between 0 and 1.
::bosdyn::api::SE3Pose Interp(const ::bosdyn::api::SE3Pose& a, const ::bosdyn::api::SE3Pose& b,
                            double factor);
// Interpolates between poses a and b at the given timestamp.
::bosdyn::api::SE3Pose Interp(const TimedPose& a, const TimedPose& b, int64_t timestamp);

typedef std::deque<TimedPose> PoseBuffer;

enum class PoseLookupResult {
    EMPTY_BUFFER,     // There is not enough data in the buffer.
    EXACT,            // Found an exact match for that time.
    INTERPOLATED,     // Used lerp/slerp between two data points.
    CLAMPED_TOO_OLD,  // Clamped to the oldest sample.
    CLAMPED_TOO_NEW   // Clamped to the newest sample.
};

// Given a pose buffer, find the pose at the given timestamp.
PoseLookupResult Lookup(const PoseBuffer& buffer, int64_t timestamp, ::bosdyn::api::SE3Pose* out);
// Add the given pose to the buffer. If the buffer is too large, the oldest
// sample will be dropped. Note that poses must be monotonically increasing.
// If this is not the case, the pose will not be added and the function
// returns false.
bool MaybeAddPose(int64_t timestamp, const ::bosdyn::api::SE3Pose& pose, PoseBuffer* buffer,
                    size_t max_poses = 100);

}  // namespace math
}  // namespace api

}  // namespace bosdyn
