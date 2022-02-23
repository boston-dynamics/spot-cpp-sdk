/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/geometry.pb.h>
#include "bosdyn/math/api_common_frames.h"

namespace bosdyn {

namespace api {

enum class ValidateFrameTreeSnapshotStatus {
    // The FrameTreeSnapshot is valid.
    VALID = 0,

    // The FrameTreeSnapshot has no edges.
    EMPTY = 1,

    // The FrameTreeSnapshot contains a cycle.
    CYCLE = 2,

    // The FrameTreeSnapshot is disjoint and has multiple roots.
    DISJOINT = 3,

    // The FrameTreeSnapshot has an empty child frame name.
    EMPTY_CHILD_FRAME_NAME = 4,

    // The FrameTreeSnapshot has an invalid parent frame name.
    UNKNOWN_PARENT_FRAME_NAME = 5,
};

ValidateFrameTreeSnapshotStatus ValidateFrameTreeSnapshot(
    const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot);

// Determine the SE(3) pose transform a_tform_b between two frames (frame a and frame b) by creating
// a local representation of the frame graph tree, and then traversing the tree to find the path
// between frames a and b and computing the transform.
bool get_a_tform_b(const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot,
                   const std::string& frame_a, const std::string& frame_b,
                   ::bosdyn::api::SE3Pose* out_a_tform_b);

// Similar to SE(3) pose conversions, except for SE(2) poses we convert the SE(3) transforms found
// by searching the FrameTreeSnapshot into the closest SE(2) pose representation.
// Note: This can only be computed from the frame tree snapshot when se2_frame_a is a gravity
// aligned frame; otherwise the conversion from the SE3 transform in the snapshot will lose height
// information.
bool get_a_tform_b(const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot,
                   const std::string& se2_frame_a, const std::string& se2_frame_b,
                   ::bosdyn::api::SE2Pose* out_a_tform_b);

// Expresses the SE(3) velocity in the frame_c.  Returns false if the frame names are not in the
// FrameTreeSnapshot, the FrameTreeSnapshot is not a well formed tree graph.
bool ExpressVelocityInNewFrame(const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot,
                                   const std::string& frame_b, const std::string& frame_c,
                                   const ::bosdyn::api::SE3Velocity& vel_of_a_in_b,
                                   ::bosdyn::api::SE3Velocity* output_vel_of_a_in_c);

// Similar to SE(3) velocity conversions, except for SE(2) velocities.  This is only valid for
// SE(2) frame names, which are "odom", "vision", and "flat_body" frames.
bool ExpressVelocityInNewFrame(const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot,
                                   const std::string& se2_frame_b, const std::string& se2_frame_c,
                                   const ::bosdyn::api::SE2Velocity& vel_of_a_in_b,
                                   ::bosdyn::api::SE2Velocity* output_vel_of_a_in_c);

// Check if transform in graph.
bool CheckSE3_a_tform_b(const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot,
                         const std::string& frame_a, const std::string& frame_b);
bool CheckSE2_a_tform_b(const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot,
                         const std::string& frame_a, const std::string& frame_b);

// Check if a frame_name is represented in a transform edge in the tree.
bool IsFrameInTree(const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot,
                      const std::string& frame_name);

// Helpers for commonly accessed transforms.
bool GetWorldTformBody(const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot,
                          ::bosdyn::api::SE3Pose* out_pose);
bool GetOdomTformBody(const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot,
                       ::bosdyn::api::SE3Pose* out_pose);

// Add an edge to a FrameTreeSnapshot.
bool AddEdgeToFrameTree(const std::string& parent_frame_name,
                            const std::string& child_frame_name, const ::bosdyn::api::SE3Pose& pose,
                            ::bosdyn::api::FrameTreeSnapshot* frame_tree_snapshot);

// Set the pose on an existing edge in a snapshot. Predominantly intended for tests.
bool SetPoseOnExistingEdge(const std::string& parent_frame_name,
                               const std::string& child_frame_name,
                               const ::bosdyn::api::SE3Pose& pose,
                               ::bosdyn::api::FrameTreeSnapshot* frame_tree_snapshot);

// Get all frames in the tree.
std::vector<std::string> ListFramesInTree(
    const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot);

// Checks if the string frame name is a known gravity aligned frame.
bool IsGravityAlignedFrameName(const std::string& frame_name);

}  // namespace api

}  // namespace bosdyn
