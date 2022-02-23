/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "api_common_frames.h"

namespace bosdyn {

namespace api {

std::string ExternalStringToInternalString(const std::string& external_frame) {
    if (external_frame == kVisionFrame) {
        return kVisionFrameInternal;
    } else if (external_frame == kOdomFrame) {
        return kOdomFrameInternal;
    } else if (external_frame == kBodyFrame) {
        return kBodyFrameInternal;
    } else if (external_frame == kGravAlignedBodyFrame){
        return kGravAlignedBodyFrame;
    }
    return external_frame;
}

std::string InternalStringToExternalString(const std::string& internal_frame) {
    if (internal_frame == kVisionFrameInternal) {
        return kVisionFrame;
    } else if (internal_frame == kOdomFrameInternal) {
        return kOdomFrame;
    } else if (internal_frame == kBodyFrameInternal) {
        return kBodyFrame;
    }
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
