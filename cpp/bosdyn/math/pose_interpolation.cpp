/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "pose_interpolation.h"

namespace bosdyn {

namespace api {
namespace math {

::bosdyn::api::Vec3 Lerp(const ::bosdyn::api::Vec3& a, const ::bosdyn::api::Vec3& b, double t) {
    return a * (1.0 - t) + b * t;
}

::bosdyn::api::Quaternion Slerp(const ::bosdyn::api::Quaternion& a, const ::bosdyn::api::Quaternion& b,
                              double t) {
    const auto& v0 = a;
    auto v1 = b;
    double dot = DotProduct(v0, v1);
    // If the dot product is negative, slerp won't take
    // the shorter path. Note that v1 and -v1 are equivalent when
    // the negation is applied to all four components. Fix by
    // reversing one quaternion.
    if (dot < 0.0) {
        v1 = -v1;
        dot = -dot;
    }

    const double DOT_THRESHOLD = 1.0 - 1e-4;
    if (dot > DOT_THRESHOLD) {
        // If the inputs are too close for comfort, linearly interpolate
        // and normalize the result.

        ::bosdyn::api::Quaternion result = v0 + t * (v1 - v0);
        result = Normalize(result);
        return result;
    }

    // Since dot is in range [0, DOT_THRESHOLD], acos is safe
    double theta_0 = acos(dot);         // theta_0 = angle between input vectors
    double theta = theta_0 * t;         // theta = angle between v0 and result
    double sin_theta = sin(theta);      // compute this value only once
    double sin_theta_0 = sin(theta_0);  // compute this value only once

    double s0 =
        cos(theta) - dot * sin_theta / sin_theta_0;  // == sin(theta_0 - theta) / sin(theta_0)
    double s1 = sin_theta / sin_theta_0;

    return (s0 * v0) + (s1 * v1);
}

::bosdyn::api::SE3Pose Interp(const ::bosdyn::api::SE3Pose& a, const ::bosdyn::api::SE3Pose& b,
                            double t) {
    ::bosdyn::api::SE3Pose interp_pose;
    interp_pose.mutable_position()->CopyFrom(Lerp(a.position(), b.position(), t));
    interp_pose.mutable_rotation()->CopyFrom(Slerp(a.rotation(), b.rotation(), t));
    return interp_pose;
}

// Interpolates between poses a and b at the given timestamp.
::bosdyn::api::SE3Pose Interp(const TimedPose& a, const TimedPose& b, int64_t timestamp) {
    // Get the interpolating value as a number between zero and one.
    double t = std::min(std::max(static_cast<double>(timestamp - a.timestamp) /
                                     static_cast<double>(b.timestamp - a.timestamp),
                                 0.0),
                        1.0);
    return Interp(a.pose, b.pose, t);
}

PoseLookupResult Lookup(const PoseBuffer& buffer, int64_t timestamp, ::bosdyn::api::SE3Pose* out) {
    if (buffer.empty()) {
        return PoseLookupResult::EMPTY_BUFFER;
    }
    size_t greatest = 0;
    size_t least = 0;

    for (size_t i = 0; i < buffer.size(); i++) {
        const auto& pose = buffer[i];
        if (pose.timestamp <= timestamp) {
            least = i;
        }
        if (pose.timestamp >= timestamp) {
            greatest = i;
            break;
        }
    }

    // We found an exact match for the pose.
    if (buffer.at(greatest).timestamp == timestamp) {
        *out = buffer.at(greatest).pose;
        return PoseLookupResult::EXACT;
    }

    // Otherwise, everything was older than the query. Return the newest.
    if (buffer.at(greatest).timestamp < timestamp) {
        *out = buffer.back().pose;
        return PoseLookupResult::CLAMPED_TOO_NEW;
    }

    // Now, we know that the timestamp is newer than the query.
    // In this case the query was too old and overran the buffer.
    if (greatest == 0) {
        *out = buffer.front().pose;
        return PoseLookupResult::CLAMPED_TOO_OLD;
    }

    // Now, we know we're between two poses.
    *out = Interp(buffer.at(least), buffer.at(greatest), timestamp);
    return PoseLookupResult::INTERPOLATED;
}

bool MaybeAddPose(int64_t timestamp, const ::bosdyn::api::SE3Pose& pose, PoseBuffer* buffer,
                    size_t max_poses) {
    // Only accept monotonically increasing timestamps.
    if (!buffer->empty() && buffer->back().timestamp >= timestamp) {
        return false;
    }

    buffer->push_back(TimedPose{timestamp, pose});
    if (buffer->size() > max_poses) {
        buffer->erase(buffer->begin());
    }
    return true;
}

}  // namespace math
}  // namespace api

}  // namespace bosdyn
