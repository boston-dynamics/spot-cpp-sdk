/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "world_object_types.h"

namespace bosdyn {
namespace api {
bool IsWorldObjectOfType(const WorldObject& obj, WorldObjectType type) {
    switch (type) {
        case WorldObjectType::WORLD_OBJECT_DRAWABLE:
            return !obj.drawable_properties().empty();
            break;
        case WorldObjectType::WORLD_OBJECT_APRILTAG:
            return obj.has_apriltag_properties();
            break;
        case WorldObjectType::WORLD_OBJECT_IMAGE_COORDINATES:
            return obj.has_image_properties();
            break;
        case WorldObjectType::WORLD_OBJECT_DOCK:
            return obj.has_dock_properties();
            break;
        case WorldObjectType::WORLD_OBJECT_UNKNOWN:
        default:
            return false;
    }
}
WorldObjectType GetTypeOfWorldObject(const WorldObject& obj) {
    if (!obj.drawable_properties().empty()) {
        return WorldObjectType::WORLD_OBJECT_DRAWABLE;
    } else if (obj.has_apriltag_properties()) {
        return WorldObjectType::WORLD_OBJECT_APRILTAG;
    } else if (obj.has_image_properties()) {
        return WorldObjectType::WORLD_OBJECT_IMAGE_COORDINATES;
    } else if (obj.has_dock_properties()) {
        return WorldObjectType::WORLD_OBJECT_DOCK;
    } else {
        return WorldObjectType::WORLD_OBJECT_UNKNOWN;
    }
}

}  // namespace api
}  // namespace bosdyn