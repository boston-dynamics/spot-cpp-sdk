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

namespace client {

// The different resource names that are managed by the lease system. Resources
// are tracked in a hierarchy, with leaf resources and parent resources which encompass
// control of multiple parts of the robot.

// Parent resources.
const std::string kBodyResource = "body";
const std::string kFullArmResource = "full-arm";

// Leaf resources.
const std::string kArmResource = "arm";
const std::string kGripperResource = "gripper";
const std::string kMobilityResource = "mobility";
const std::string kFanResource = "fan";

}  // namespace client

}  // namespace bosdyn
