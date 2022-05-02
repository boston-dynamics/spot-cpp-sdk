<!--
Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.

Downloading, reproducing, distributing or otherwise using the SDK Software
is subject to the terms and conditions of the Boston Dynamics Software
Development Kit License (20191101-BDSDK-SL).
-->

# Spot C++ SDK Release Notes

## Spot C++ SDK version 3.1.1 BETA

No changes from 3.1.0.


## Spot C++ SDK version 3.1.0 BETA

Please refer to 3.1.0 section in [Python SDK documentation](https://dev.bostondynamics.com/docs/release_notes) for changes in the protobuf definitions in this release.

### New Features

#### New Clients

Added DataAcquisitionStoreClient for communication with DataAcquisitionStore service.

Added GripperCameraParamClient for communication with new GripperCameraParam service.

Added LicenseClient for communication with License service.

#### C++ Helpers

Added `IntersectRayPlane` helper function in proto_math.

Added image source name helper functions.

Added world object type helper functions.

### Bug Fixes and Improvements

#### GraphNav
Fixed conversions of status enumerations to `std::error_code` in the messages `ClearGraphResponse`, `NavigateToAnchorResponse`, `UploadWaypointSnapshotResponse`, `UploadGraphResponse`.

#### Image Client
Added `resize_ratio` argument in `BuildImageRequest` function.


## Spot C++ SDK version 3.0.3 BETA

No changes from 3.0.2.


## Spot C++ SDK version 3.0.2 BETA

Initial **Beta** release of the Spot C++ SDK.

