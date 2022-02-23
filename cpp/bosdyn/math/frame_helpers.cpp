/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "frame_helpers.h"

#include <queue>
#include "bosdyn/math/proto_math.h"

namespace bosdyn {

namespace api {

namespace {

std::vector<::bosdyn::api::FrameTreeSnapshot::ParentEdge> walk_tree_to_root(
    const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot, const std::string& leaf_frame_name) {
    // Assume a valid tree.
    std::vector<::bosdyn::api::FrameTreeSnapshot::ParentEdge> ret;
    const auto& child_to_parent_edge_map = frame_tree_snapshot.child_to_parent_edge_map();
    std::string cur_frame = leaf_frame_name;
    while (true) {
        const auto& parent_edge_it = child_to_parent_edge_map.find(cur_frame);
        const auto& parent_edge = parent_edge_it->second;
        if (parent_edge.parent_frame_name().empty()) {
            break;
        }
        ret.push_back(parent_edge);
        cur_frame = parent_edge.parent_frame_name();
    }
    return ret;
}

::bosdyn::api::SE3Pose accumulate_transforms(
    const std::vector<::bosdyn::api::FrameTreeSnapshot::ParentEdge>& path_to_root) {
    ::bosdyn::api::SE3Pose ret = Identity();
    for (const auto& parent_edge : path_to_root) {
        ret = parent_edge.parent_tform_child() * ret;
    }
    return ret;
}

ValidateFrameTreeSnapshotStatus find_tree_root(
    const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot, const std::string& leaf_frame_name,
    std::string* root_frame_name) {
    std::string cur_frame_name = leaf_frame_name;
    if (leaf_frame_name.empty()) {
        return ValidateFrameTreeSnapshotStatus::EMPTY_CHILD_FRAME_NAME;
    }
    std::set<std::string> visited = std::set<std::string>();
    visited.insert(leaf_frame_name);
    const auto& child_to_parent_edge_map = frame_tree_snapshot.child_to_parent_edge_map();
    while (true) {
        auto it = child_to_parent_edge_map.find(cur_frame_name);
        if (it == child_to_parent_edge_map.end()) {
            return ValidateFrameTreeSnapshotStatus::UNKNOWN_PARENT_FRAME_NAME;
        }
        const auto& parent_edge = it->second;
        const std::string& parent_frame_name = parent_edge.parent_frame_name();
        if (parent_frame_name.empty()) {
            break;
        }
        auto insert_result = visited.insert(parent_frame_name);
        if (!insert_result.second) {
            return ValidateFrameTreeSnapshotStatus::CYCLE;
        }
        cur_frame_name = parent_frame_name;
    }
    if (root_frame_name) {
        *root_frame_name = cur_frame_name;
    }
    return ValidateFrameTreeSnapshotStatus::VALID;
}

}  // namespace

ValidateFrameTreeSnapshotStatus ValidateFrameTreeSnapshot(
    const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot) {
    std::string root_frame_name;
    const auto& child_to_parent_edge_map = frame_tree_snapshot.child_to_parent_edge_map();
    if (child_to_parent_edge_map.empty()) {
        return ValidateFrameTreeSnapshotStatus::EMPTY;
    }
    for (const auto& key_value_pair : child_to_parent_edge_map) {
        if (key_value_pair.first.empty()) {
            return ValidateFrameTreeSnapshotStatus::EMPTY_CHILD_FRAME_NAME;
        }
        std::string cur_root_frame_name;
        ValidateFrameTreeSnapshotStatus status =
            find_tree_root(frame_tree_snapshot, key_value_pair.first, &cur_root_frame_name);
        if (status != ValidateFrameTreeSnapshotStatus::VALID) {
            return status;
        }
        if (root_frame_name.empty()) {
            root_frame_name = cur_root_frame_name;
        } else {
            if (root_frame_name != cur_root_frame_name) {
                return ValidateFrameTreeSnapshotStatus::DISJOINT;
            }
        }
    }
    return ValidateFrameTreeSnapshotStatus::VALID;
}

bool get_a_tform_b(const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot,
                   const std::string& frame_a, const std::string& frame_b,
                   ::bosdyn::api::SE3Pose* out_a_tform_b) {
    ValidateFrameTreeSnapshotStatus status = ValidateFrameTreeSnapshot(frame_tree_snapshot);
    if (status != ValidateFrameTreeSnapshotStatus::VALID) {
        return false;
    }
    const auto& child_to_parent_edge_map = frame_tree_snapshot.child_to_parent_edge_map();
    if (!child_to_parent_edge_map.count(frame_a)) {
        return false;
    }
    if (!child_to_parent_edge_map.count(frame_b)) {
        return false;
    }
    // At this point, there should be a way to get from a to b.
    if (out_a_tform_b) {
        auto inverse_edges = walk_tree_to_root(frame_tree_snapshot, frame_a);
        auto forward_edges = walk_tree_to_root(frame_tree_snapshot, frame_b);

        ::bosdyn::api::SE3Pose inverse_transform = accumulate_transforms(inverse_edges);
        ::bosdyn::api::SE3Pose forward_transform = accumulate_transforms(forward_edges);
        *out_a_tform_b = ~inverse_transform * forward_transform;
    }

    return true;
}

bool get_a_tform_b(const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot,
                   const std::string& se2_frame_a, const std::string& se2_frame_b,
                   ::bosdyn::api::SE2Pose* out_a_tform_b) {
    // Get the SE3 Pose from the snapshot representing frame_a_tform_frame_b.
    ::bosdyn::api::SE3Pose se3_a_tform_b;
    if (!get_a_tform_b(frame_tree_snapshot, ::bosdyn::api::SE2StringToClosestSE3String(se2_frame_a),
                       ::bosdyn::api::SE2StringToClosestSE3String(se2_frame_b), &se3_a_tform_b)) {
        // Failed to find a representative SE3 Pose for the same transform.
        return false;
    }
    // Convert the SE3 pose to an SE2 pose. This function will also check that the SE(3) pose frame
    if (!SafeFlatten(se3_a_tform_b, se2_frame_a, out_a_tform_b)) {
        // Failed to find the closest se2 transform, clear out the pointer to the output a_tform_b
        out_a_tform_b->Clear();
        return false;
    }

    // Successfully found the SE(2) transform a_tform_b!
    return true;
}

bool ExpressVelocityInNewFrame(const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot,
                                   const std::string& frame_b, const std::string& frame_c,
                                   const ::bosdyn::api::SE3Velocity& vel_of_a_in_b,
                                   ::bosdyn::api::SE3Velocity* output_vel_of_a_in_c) {
    // Find the SE(3) pose in the frame tree snapshot that represents c_tform_b.
    ::bosdyn::api::SE3Pose se3_c_tform_b;
    if (!get_a_tform_b(frame_tree_snapshot, frame_c, frame_b, &se3_c_tform_b)) {
        // If the SE3Pose for c_tform_b does not exist in the frame tree snapshot,
        // then we cannot transform the velocity.
        return false;
    }
    auto c_adjoint_b = Adjoint(se3_c_tform_b);
    output_vel_of_a_in_c->CopyFrom(TransformVelocity(c_adjoint_b, vel_of_a_in_b));
    return true;
}

bool ExpressVelocityInNewFrame(const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot,
                                   const std::string& se2_frame_b, const std::string& se2_frame_c,
                                   const ::bosdyn::api::SE2Velocity& vel_of_a_in_b,
                                   ::bosdyn::api::SE2Velocity* output_vel_of_a_in_c) {
    // Find the closest SE(2) pose in the frame tree snapshot that represents c_tform_b.
    // This function will also check that the SE(3) pose frame name is considered to be a
    // gravity aligned frame.
    ::bosdyn::api::SE2Pose se2_c_tform_b;
    if (!get_a_tform_b(frame_tree_snapshot, se2_frame_c, se2_frame_b, &se2_c_tform_b)) {
        // If the SE3Pose for c_tform_b does not exist in the frame tree snapshot,
        // then we cannot transform the velocity.
        return false;
    }
    // Transform the velocity into the new frame to get vel_of_a_in_c.
    auto c_adjoint_b = Adjoint(se2_c_tform_b);
    output_vel_of_a_in_c->CopyFrom(TransformVelocity(c_adjoint_b, vel_of_a_in_b));
    return true;
}

bool CheckSE3_a_tform_b(const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot,
                         const std::string& frame_a, const std::string& frame_b) {
    ::bosdyn::api::SE3Pose dummy_a_tform_b;
    return get_a_tform_b(frame_tree_snapshot, frame_a, frame_b, &dummy_a_tform_b);
}

bool CheckSE2_a_tform_b(const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot,
                         const std::string& frame_a, const std::string& frame_b) {
    ::bosdyn::api::SE2Pose dummy_a_tform_b;
    return get_a_tform_b(frame_tree_snapshot, frame_a, frame_b, &dummy_a_tform_b);
}

bool IsFrameInTree(const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot,
                      const std::string& frame_name) {
    return frame_tree_snapshot.child_to_parent_edge_map().count(frame_name) > 0;
}

bool GetOdomTformBody(const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot,
                       ::bosdyn::api::SE3Pose* out_pose) {
    return get_a_tform_b(frame_tree_snapshot, ::bosdyn::api::kOdomFrame, ::bosdyn::api::kBodyFrame,
                         out_pose);
}

bool GetWorldTformBody(const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot,
                          ::bosdyn::api::SE3Pose* out_pose) {
    return get_a_tform_b(frame_tree_snapshot, ::bosdyn::api::kVisionFrame, ::bosdyn::api::kBodyFrame,
                         out_pose);
}

bool AddEdgeToFrameTree(const std::string& parent_frame_name,
                            const std::string& child_frame_name, const ::bosdyn::api::SE3Pose& pose,
                            ::bosdyn::api::FrameTreeSnapshot* frame_tree_snapshot) {
    if (IsFrameInTree(*frame_tree_snapshot, child_frame_name)) {
        return false;
    }
    // Existence of parent is not checked, since there are no guarantees of construction ordering.

    auto* child_to_parent_edge_map = frame_tree_snapshot->mutable_child_to_parent_edge_map();
    auto& parent_edge = (*child_to_parent_edge_map)[child_frame_name];
    parent_edge.set_parent_frame_name(parent_frame_name);
    parent_edge.mutable_parent_tform_child()->CopyFrom(pose);
    return true;
}

bool SetPoseOnExistingEdge(const std::string& parent_frame_name,
                               const std::string& child_frame_name,
                               const ::bosdyn::api::SE3Pose& pose,
                               ::bosdyn::api::FrameTreeSnapshot* frame_tree_snapshot) {
    bool mutated_frame = false;
    auto* frame_to_parent_edge_map = frame_tree_snapshot->mutable_child_to_parent_edge_map();
    for (const auto& frame_to_parent_edge : *frame_to_parent_edge_map) {
        if (!child_frame_name.empty() && frame_to_parent_edge.first != child_frame_name) {
            continue;
        }
        ::bosdyn::api::FrameTreeSnapshot_ParentEdge& parent_edge =
            frame_to_parent_edge_map->at(child_frame_name);
        if (!parent_frame_name.empty() && parent_edge.parent_frame_name() != parent_frame_name) {
            continue;
        }

        parent_edge.mutable_parent_tform_child()->CopyFrom(pose);
        mutated_frame = true;
    }
    return mutated_frame;
}

std::vector<std::string> ListFramesInTree(
    const ::bosdyn::api::FrameTreeSnapshot& frame_tree_snapshot) {
    std::vector<std::string> frame_names;
    for (const auto& k : frame_tree_snapshot.child_to_parent_edge_map()) {
        frame_names.push_back(k.first);
    }
    return frame_names;
}

bool IsGravityAlignedFrameName(const std::string& frame_name) {
    if ((frame_name == ::bosdyn::api::kGravAlignedBodyFrame) ||
        (frame_name == ::bosdyn::api::kVisionFrame) || (frame_name == ::bosdyn::api::kOdomFrame)) {
        return true;
    }
    return false;
}

}  // namespace api

}  // namespace bosdyn
