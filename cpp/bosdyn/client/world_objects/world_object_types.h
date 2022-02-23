/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/world_object.pb.h>

namespace bosdyn {
namespace api {
// Determines if the given World Object is of the given type (helper).
bool IsWorldObjectOfType(const WorldObject& obj, WorldObjectType type);
// Gets the type of the given World Object (helper).
WorldObjectType GetTypeOfWorldObject(const WorldObject& obj);
}  // namespace api
}  // namespace bosdyn