<!--
Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.

Downloading, reproducing, distributing or otherwise using the SDK Software
is subject to the terms and conditions of the Boston Dynamics Software
Development Kit License (20191101-BDSDK-SL).
-->

# Spot C++ SDK Release Notes

## Spot C++ SDK version 3.3.2 BETA

No changes from 3.3.0

## Spot C++ SDK version 3.3.1 BETA

No changes from 3.3.0.

### Upcoming Removals

In the upcoming release 4.0, we plan to change the encoding for real-valued fields in local grids to RAW instead of RLE (Run-Length Encoded) and provide client helpers for decoding. Make sure your code handles the `encoding` field in `LocalGridResponse` correctly.

## Spot C++ SDK version 3.3.0 BETA

### New Features

Added `KeepAlive` client (BETA) to support corresponding proto additions.

Added `LogStatus` client to support corresponding proto additions.

Added `Manipulation API` Client.

Added `InverseKinematics` client to support corresponding proto additions.

### Bug Fixes and Improvements

#### API

Please see the Release Notes in https://dev.bostondynamics.com for a description of the API changes included in release 3.3.0.

#### SDK

Improved conversion of `grpc::StatusCode::UNAVAILABLE` error statuses to SDK error codes in `rpc_error_code.cpp`.

Added `AutoReturn` client error codes for `StartResponse` Status enumeration.

Return errors from converting data chunk to a response in `Autowalk` client `CompileAutowalk` RPC, instead of returning the more generic `GenericSDKError` errors.

Data Chunk

- Reduced chunk size from 4MB to 2MB.
- Added call to reserve the string capacity to avoid the frequent memory reallocations.

Mission Client

- Added `LoadMissionAsChunks2` method to support bidirectional streaming in the `LoadMissionAsChunks` RPC.
- Added `GetMissionAsChunks` methods to support the corresponding RPC.

Robot Command

- Added helper methods in robot_command_builder for using the Spot Arm.
- Added `BodyPose` method in robot_command_builder for creating a `BodyControlParams.BodyPose` from a single desired `body_pose` relative to `frame_name`.
- Added a set of helper functions in `robot_command_helpers.h` to be used with a robot command client.

Added `ProcessAndInitiateRequestStreamAsyncCall` method in `ServiceClient`, which packages a full request message that includes an API `RequestHeader` into multiple `DataChunk` messages that are streamed directly to the robot via a request-streaming RPC.

Spot CAM Clients

- Compositor Client
  - Added more versions of `SetIrMeterOverlay` method with various argument combinations.
  - Added `GetIrMeterOverlay` methods.
- PTZ Client
  - Added methods for `GetPtzFocusState`, `SetPtzFocusState` RPCs.
- StreamQuality Client
  - Added more versions of `SetStreamParams` method with various argument combinations.

Added support for `WORLD_OBJECT_STAIRCASE` and `WORLD_OBJECT_USER_NOGO` types in `WorldObjects` Client.

Added `NowSec` and `NowTimePoint` methods in common time helper methods.

Added various arm frame names and the stairs base frame names for world objects in common API frames.

Added `EigenFromApiProto/EigenToApiProto`, `FromRoll/ToRoll`, `FromPitch/ToPitch` methods in common proto math helper methods.

### Deprecations

Please see the Release Notes in https://dev.bostondynamics.com for a description of the API deprecations included in release 3.3.0.

### Breaking Changes

### Dependencies

### Known Issues

### Sample Code

#### New

[Arm Grasp Carry Overrides](../cpp/examples/arm_grasp_carry_overrides/README.md): Sets arm grasp state and carry state overrides while moving the arm around.

[Arm Manipulation API Walk To Object](../cpp/examples/arm_manipulation_api_walk_to/README.md): Sends a `WalkToObjectRayInWorld` command to the robot via the Manipulation API.

[Inverse Kinematics Reachability](../cpp/examples/inverse_kinematics_reachability/README.md): Shows how to use the Inverse Kinematics Service API to make reachability queries for random arm poses.

#### Updated

[Query Autowalk Status](../cpp/examples/query_autowalk_status/README.md): Modified the example to contain a mission_name command line argument. If the mission_name is not specified, the script attempts to guess the mission name.

## Spot C++ SDK version 3.2.3 BETA

No changes from 3.2.2 BETA.

## Spot C++ SDK version 3.2.2 BETA

### Bug Fixes and Improvements

Updated `Robot` and `ClientSDK` classes to enable applications to specify the `LeaseWallet` used in instantiated clients.

Added `GetSystemLog` methods in `HealthClient`.

## Spot C++ SDK version 3.2.1 BETA

No changes from 3.2.0.

## Spot C++ SDK version 3.2.0 BETA

### New Features

#### Autowalk Service

Enables API clients to specify high level autonomous behaviors for Spot using an easily editable format. The autowalk format is a list of actions and their associated locations. Using this service, users can program the robot to “go to location A, perform action A, go to location B, perform action B, etc”. The autowalk service compiles the autowalk into a behavior tree that can then be uploaded to the robot and played using the mission service. Previously this feature was only available on the tablet, but now it is a service for all client applications.

#### Graph Nav – Area Callbacks

Enables users to register a callback that is called in certain areas of the map during navigation. These “Area Callbacks” can instruct the robot to wait until the area is safe to cross (such as a crosswalk), take control of the robot and perform an action (such as opening a door), or perform a background action while in a certain area of the map (such as flashing lights or playing sounds).
This enables integration with the [Graph Nav](concepts/autonomy/graphnav_service.md) navigation system to extend its capabilities in terms of safety and new actions while navigating. See the [Area Callback](concepts/autonomy/area_callbacks.md) documentation for more details.

### Bug Fixes and Improvements

#### API

Please see the Release Notes in https://dev.bostondynamics.com for a description of the API changes included in release 3.2.0.

#### SDK

Added `RetryableUnavailableError` RPCErrorCode and `RetryableRPCCondition` to check whether a failed RPC should be retried.

Fixed issues in the `ConvertGRPCStatus` method that converts grpc:Status to our `::bosdyn::common::Status`.

Added `LoadMissionAsChunks` and `LoadMissionAsChunksAsync` methods in `MissionClient`.

Added `FanPowerCommandAsync` and `FanPowerCommand` methods in `PowerClient`.

Added `UpdateSecureChannelPort` in Robot class to update the port used for creating secure channels, instead of using the default 443

Fixed ordering of adding RPC calls to the gRPC completion queue in ServiceClient class to fix a memory leak caused if RPCs are completed right away.

Added `InitiateRequestStreamAsyncCallWithChunking` and `InitiateRequestResponseStreamAsyncCallWithChunking` helper functions in `ServiceClient` class to be used when directly streaming DataChunks.

Added `SetupRequestStreamCall` and `SetupRequestResponseStreamCall` helper functions in `ServiceClient` class that initiates a request stream async call.

Added `Is` helper function in `Status` class to check if this `Status` matches a certain error enum.

### Deprecations

Please see the Release Notes in https://dev.bostondynamics.com for a description of the API deprecations included in release 3.2.0.

### Breaking Changes

Updated `StringToDataChunks`and `MessageToDataChunks` method signatures in data_chunking to accept a vector of `::bosdyn::api::DataChunk` instead of a vector of `std::unique_ptr<::bosdyn::api::DataChunk>`.

### Dependencies

### Known Issues

### Sample Code

[Query Autowalk Status](../cpp/examples/query_autowalk_status/README.md): Example that loads an Autowalk and GraphNav map to the robot from a file. It then queries the status of elements and actions in the walk as the Autowalk mission plays.

## Spot C++ SDK version 3.1.2 BETA

No changes from 3.1.0.

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
