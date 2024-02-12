/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "api_common_frames.h"

#include <optional>
#include <string>
#include <unordered_map>


namespace bosdyn {

namespace api {

namespace {
/// Spot's Internal to External Frame naming conversion.
const std::unordered_map<std::string, std::string> kSpotInternalToExternalFrameMap{
    {kVisionFrameInternal, kVisionFrame},
    {kOdomFrameInternal, kOdomFrame},
    {kBodyFrameInternal, kBodyFrame}};

/// Spot's External to Internal Frame naming conversion.
const std::unordered_map<std::string, std::string> kSpotExternalToInternalFrameMap{
    {kVisionFrame, kVisionFrameInternal},
    {kOdomFrame, kOdomFrameInternal},
    {kBodyFrame, kBodyFrameInternal},
    {kGravAlignedBodyFrame, kGravAlignedBodyFrame}};

}  // namespace

std::string ExternalStringToInternalString(const std::string& external_frame,
                                           const RobotSpecies robot_species) {
    auto get_internal_string =
        [](const std::unordered_map<std::string, std::string>& external_to_internal_frame_map,
           const std::string& external_frame) -> std::optional<std::string> {
        // Find the frame name conversion in the map.
        if (auto itr{external_to_internal_frame_map.find(external_frame)};
            itr != external_to_internal_frame_map.end()) {
            return itr->second;
        }
        return std::nullopt;
    };

    switch (robot_species) {
        case RobotSpecies::kSpot:
            if (const auto internal_string{
                    get_internal_string(kSpotExternalToInternalFrameMap, external_frame)};
                internal_string.has_value()) {
                return internal_string.value();
            }
            break;
    };

    return external_frame;
}

std::string InternalStringToExternalString(const std::string& internal_frame,
                                           const RobotSpecies robot_species) {
    auto get_external_string =
        [](const std::unordered_map<std::string, std::string>& internal_to_external_frame_map,
           const std::string& internal_frame) -> std::optional<std::string> {
        // Find the frame name conversion in the map.
        if (auto itr{internal_to_external_frame_map.find(internal_frame)};
            itr != internal_to_external_frame_map.end()) {
            return itr->second;
        }
        return std::nullopt;
    };

    switch (robot_species) {
        case RobotSpecies::kSpot:
            if (const auto external_string{
                    get_external_string(kSpotInternalToExternalFrameMap, internal_frame)};
                external_string.has_value()) {
                return external_string.value();
            }
            break;
    };

    return internal_frame;
}

std::string SE2StringToClosestSE3String(const std::string& se2_frame) {
    if (se2_frame == kGravAlignedBodyFrame) {
        return kBodyFrame;
    }
    // The other two "se2_frames" that are used are vision and odom, which have the same names.
    return se2_frame;
}

std::string FrameEnumToExternalString(CommonApiFrames frame) {
    switch (frame) {
        case CommonApiFrames::kOdomFrame:
            return kOdomFrame;
        case CommonApiFrames::kVisionFrame:
            return kVisionFrame;
        case CommonApiFrames::kBodyFrame:
            return kBodyFrame;
    }
    // Quieting the compiler.
    // https://stackoverflow.com/questions/33607284/control-reaches-end-of-non-void-function-with-fully-handled-case-switch-over-a
    return kUnknownFrame;
}

std::string ClosestSE2StringFromFrameEnum(CommonApiFrames frame) {
    switch (frame) {
        case CommonApiFrames::kOdomFrame:
            return kOdomFrame;
        case CommonApiFrames::kVisionFrame:
            return kVisionFrame;
        case CommonApiFrames::kBodyFrame:
            return kGravAlignedBodyFrame;
    }
    // Quieting the compiler.
    // https://stackoverflow.com/questions/33607284/control-reaches-end-of-non-void-function-with-fully-handled-case-switch-over-a
    return kUnknownFrame;
}

std::string CombineNameAndId(const std::string& frame_name, int id) {
    return frame_name + std::to_string(id);
}

std::string ChannelToCameraFrameName(const std::string& channel_name) {
    if (channel_name == "frontright") {
        return ::bosdyn::api::kFrontRightFisheyeFrame;
    } else if (channel_name == "frontleft") {
        return ::bosdyn::api::kFrontLeftFisheyeFrame;
    } else if (channel_name == "right") {
        return ::bosdyn::api::kRightFisheyeFrame;
    } else if (channel_name == "left") {
        return ::bosdyn::api::kLeftFisheyeFrame;
    } else if (channel_name == "back") {
        return ::bosdyn::api::kBackFisheyeFrame;
    } else {
        return ::bosdyn::api::kUnknownFrame;
    }
}

}  // namespace api

}  // namespace bosdyn
