/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <array>
#include <string>

namespace bosdyn {

namespace api {

enum class RobotSpecies {
    kSpot = 0,

};

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
// DEPRECATED.  In software version <4.0, we incorrectly used "link_wr1" in the kinematic_state's
// transforms_snapshot instead of the URDF's link name of "arm0.link_wr1".  Starting in 4.0 we added
// the arm0 prefix. New code should use kWr1Frame instead.
[[deprecated("Use kWr1Frame instead.")]] const std::string kWristFrame = "link_wr1";
// If an arm is attached, this frame is attached to the robot's distal wrist link and represents
// that links position and orientation. The origin of the frame is at the end of the link on its
// rotational axis.  It's x-axis is aligned with the rotational axis.
const std::string kWr1Frame = "arm0.link_wr1";

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

/// @brief Converts frame name of specified robot species from External to Internal.
/// @param [in] external_frame
/// @param [in] robot_species Default is Spot.
/// @return Internal frame name.
std::string ExternalStringToInternalString(const std::string& external_frame,
                                           const RobotSpecies robot_species = RobotSpecies::kSpot);

/// @brief Converts frame name of specified robot species from Internal to External.
/// @param [in] internal_frame
/// @param [in] robot_species Default is Spot.
/// @return External frame name.
std::string InternalStringToExternalString(const std::string& internal_frame,
                                           const RobotSpecies robot_species = RobotSpecies::kSpot);

std::string SE2StringToClosestSE3String(const std::string& se2_frame);

std::string CombineNameAndId(const std::string& frame_name, int id);

std::string ChannelToCameraFrameName(const std::string& channel_name);

// Base names for BD robot species

// Base frame names for world objects.
const char kFiducialFrame[] =
    "fiducial_";  // This should always be suffixed with the fiducial tag id.
const char kFilteredFiducialFrame[] =
    "filtered_fiducial_";  // This should always be suffixed with the fiducial tag id.
const char kDoorFrame[] = "door_";
const char kImageCoordsFrame[] = "image_coords_";
const char kDockName[] = "dock_";
const char kStairsName[] = "stairs_";

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

// Various arm frame names
const char kToolFrame[] = "tool";
const char kDesiredToolFrame[] = "desired_tool";
const char kTaskFrame[] = "task";
const char kDesiredToolAtEndFrame[] = "desired_tool_at_end";
const char kMeasuredToolAtStartFrame[] = "measured_tool_at_start";
const char kGazeTargetFrame[] = "gaze_target";

// Foot related frames
const char kFrontLeftFootFrame[] = "fl_foot";
const char kFrontRightFootFrame[] = "fr_foot";
const char kHindLeftFootFrame[] = "hl_foot";
const char kHindRightFootFrame[] = "hr_foot";
const std::array<const char*, 4> kFootFrames{kFrontLeftFootFrame, kFrontRightFootFrame,
                                             kHindLeftFootFrame, kHindRightFootFrame};
const char kFeetCenterFrame[] = "feet_center";

// Various frame names used for localization with respect to the Earth.
const std::string kEarthFrame = "ecef";  // Earth Centered Earth Fixed
const std::string kEnuFrame = "enu";     // East North Up
}  // namespace api

}  // namespace bosdyn
