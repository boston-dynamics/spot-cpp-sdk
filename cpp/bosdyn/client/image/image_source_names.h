/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

namespace bosdyn {
namespace client {

// clang-format off
enum class BodyCamera {
    BACK,
    FRONTLEFT,
    FRONTRIGHT,
    LEFT,
    RIGHT
};
// clang-format on

constexpr const char* GetFisheyeImageName(BodyCamera camera) {
    switch (camera) {
        case BodyCamera::BACK: {
            return "back_fisheye_image";
        }
        case BodyCamera::FRONTLEFT: {
            return "frontleft_fisheye_image";
        }
        case BodyCamera::FRONTRIGHT: {
            return "frontright_fisheye_image";
        }
        case BodyCamera::LEFT: {
            return "left_fisheye_image";
        }
        case BodyCamera::RIGHT: {
            return "right_fisheye_image";
        }
    }
    // To appease `-Werror=return-type`.
    return "case_unhandled";
}

constexpr const char* GetDepthName(BodyCamera camera) {
    switch (camera) {
        case BodyCamera::BACK: {
            return "back_depth";
        }
        case BodyCamera::FRONTLEFT: {
            return "frontleft_depth";
        }
        case BodyCamera::FRONTRIGHT: {
            return "frontright_depth";
        }
        case BodyCamera::LEFT: {
            return "left_depth";
        }
        case BodyCamera::RIGHT: {
            return "right_depth";
        }
    }
    // To appease `-Werror=return-type`.
    return "case_unhandled";
}

constexpr const char* GetDepthInVisualName(BodyCamera camera) {
    switch (camera) {
        case BodyCamera::BACK: {
            return "back_depth_in_visual_frame";
        }
        case BodyCamera::FRONTLEFT: {
            return "frontleft_depth_in_visual_frame";
        }
        case BodyCamera::FRONTRIGHT: {
            return "frontright_depth_in_visual_frame";
        }
        case BodyCamera::LEFT: {
            return "left_depth_in_visual_frame";
        }
        case BodyCamera::RIGHT: {
            return "right_depth_in_visual_frame";
        }
    }
    // To appease `-Werror=return-type`.
    return "case_unhandled";
}

// Sources on spot robots with an arm.
static constexpr char kHandColorImage[] = "hand_color_image";
static constexpr char kHandColorInHandDepthFrame[] = "hand_color_in_hand_depth_frame";
static constexpr char kHandDepth[] = "hand_depth";
static constexpr char kHandDepthInHandColorFrame[] = "hand_depth_in_hand_color_frame";
static constexpr char kHandImage[] = "hand_image";

}  // namespace client
}  // namespace bosdyn