/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "world_object_types.h"
#include "bosdyn/api/world_object.pb.h"

namespace bosdyn {
namespace api {
bool IsWorldObjectOfType(const WorldObject& obj, WorldObjectType type) {
    switch (type) {
        case WorldObjectType::WORLD_OBJECT_DRAWABLE:
            return !obj.drawable_properties().empty();
        case WorldObjectType::WORLD_OBJECT_APRILTAG:
            return obj.has_apriltag_properties();
        case WorldObjectType::WORLD_OBJECT_IMAGE_COORDINATES:
            return obj.has_image_properties();
        case WorldObjectType::WORLD_OBJECT_DOCK:
            return obj.has_dock_properties();
        case WorldObjectType::WORLD_OBJECT_STAIRCASE:
            return obj.has_staircase_properties();
        case WorldObjectType::WORLD_OBJECT_UNKNOWN:
            return false;
        case WORLD_OBJECT_USER_NOGO:
            return obj.has_nogo_region_properties();
        case WorldObjectType_INT_MIN_SENTINEL_DO_NOT_USE_:
        case WorldObjectType_INT_MAX_SENTINEL_DO_NOT_USE_:
            return false;
    }
    // Unreachable.
    return false;
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
    } else if (obj.has_staircase_properties()) {
        return WorldObjectType::WORLD_OBJECT_STAIRCASE;
    } else if (obj.has_nogo_region_properties()) {
        return WorldObjectType::WORLD_OBJECT_USER_NOGO;
    } else {
        return WorldObjectType::WORLD_OBJECT_UNKNOWN;
    }
}

}  // namespace api
}  // namespace bosdyn
