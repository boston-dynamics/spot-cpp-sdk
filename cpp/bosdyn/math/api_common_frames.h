/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <string>

namespace bosdyn {

namespace api {

enum class CommonApiFrames { kVisionFrame, kOdomFrame, kBodyFrame };

// Common frame names for SE(3) Math.
// Note, that for SE(2) math, only the frames "flat_body", "vision", and "odom" are
// currently used since these are gravity-aligned frames and can be safely converted
// between an SE(2) <--> SE(3) representation.
const std::string kVisionFrame = "vision";
const std::string kOdomFrame = "odom";
const std::string kBodyFrame = "body";
// If an arm is attached to the robot, the hand frame is a frame located with it's origin slightly
// in front of the gripper's palm plate aligned with wrist's orientation.
const std::string kHandFrame = "hand";
const std::string kWristFrame = "link_wr1";

// Note that the frame name for a gravity aligned body frame is "flat_body" to create a shorter
// string identifier that can be handled by all common compilers.
const std::string kGravAlignedBodyFrame = "flat_body";
const std::string kGroundPlaneEstimateFrame = "gpe";
const std::string kUnknownFrame = "unknown";

// Common frame names as understood by the PoseGraph.
const std::string kVisionFrameInternal = "vw";
const std::string kOdomFrameInternal = "ow";
const std::string kBodyFrameInternal = "base";

std::string FrameEnumToExternalString(CommonApiFrames frame);

std::string ClosestSE2StringFromFrameEnum(CommonApiFrames frame);

std::string ExternalStringToInternalString(const std::string& external_frame);

std::string InternalStringToExternalString(const std::string& internal_frame);

std::string SE2StringToClosestSE3String(const std::string& se2_frame);

std::string CombineNameAndId(const std::string& frame_name, int id);

std::string ChannelToCameraFrameName(const std::string& channel_name);

// Base frame names for world objects.
const char kFiducialFrame[] =
    "fiducial_";  // This should always be suffixed with the fiducial tag id.
const char kFilteredFiducialFrame[] =
    "filtered_fiducial_";  // This should always be suffixed with the fiducial tag id.
const char kDoorFrame[] = "door_";
const char kImageCoordsFrame[] = "image_coords_";
const char kDockName[] = "dock_";

// Sensor frame names. These are consistent for internal and external.
const char kFrontLeftFisheyeFrame[] = "frontleft_fisheye";
const char kFrontRightFisheyeFrame[] = "frontright_fisheye";
const char kLeftFisheyeFrame[] = "left_fisheye";
const char kRightFisheyeFrame[] = "right_fisheye";
const char kBackFisheyeFrame[] = "back_fisheye";

// Local Grid frame names.
const char kTerrainLocalGridFrame[] = "terrain_local_grid_corner";
const char kTerrainValidLocalGridFrame[] = "terrain_valid_local_grid_corner";
const char kTerrainIntensityLocalGridFrame[] = "terrain_intensity_local_grid_corner";
const char kObstacleDistanceLocalGridFrame[] = "obstacle_distance_local_grid_corner";
const char kNoStepLocalGridFrame[] = "no_step_local_grid_corner";

// Point Cloud frame names
const char kSensorOriginPrefixFrame[] = "sensor_origin_";
const char kLidarSensorOrigin[] = "sensor_origin_lidar";

// Arm frame names.
// const char kManipulatorFrame[] = "manipulator" // ???

const char kManipulationWalktoRaycastIntersection[] = "walkto_raycast_intersection";
const char kManipulationGraspPlanningSolution[] = "grasp_planning_solution";
const char kManipulationGraspNearestObject[] = "gripper_nearest_object";
const char kManipulationGraspTargetObject[] = "grasp_target_object";

}  // namespace api

}  // namespace bosdyn
