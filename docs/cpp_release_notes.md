<!--
Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.

Downloading, reproducing, distributing or otherwise using the SDK Software
is subject to the terms and conditions of the Boston Dynamics Software
Development Kit License (20191101-BDSDK-SL).
-->

# Spot C++ SDK Release Notes

## Spot C++ SDK version 4.1.0 BETA

### New Features

#### Graph Nav

- Added support for uploading and downloading [graphs](../protos/bosdyn/api/graph_nav/map.proto#graph) larger than 4 MB has been added via the [UploadGraphStreaming](../protos/bosdyn/api/graph_nav/graph_nav_service.proto#uploadgraphstreaming) and [DownloadGraphStreaming](../protos/bosdyn/api/graph_nav/graph_nav_service.proto#downloadgraphstreaming) RPCs, respectively. Please see the corresponding client implementation in [GraphNavClient](../cpp/bosdyn/client/graph_nav/graph_nav_client.cpp).

### Bug Fixes and Improvements

#### API

Please see the [General Release Notes](https://dev.bostondynamics.com/docs/release_notes) for a description of the API changes included in release 4.1.0.

#### SDK

Math & Frame Helpers

- A new reference frame, "feet_center", has been added. Please see the [General Release Notes](https://dev.bostondynamics.com/docs/release_notes) for more information.

- The supplied `z_height` argument in the `SafeInflate` method is now actually used. Previously, the `z_height` argument was not passed to the inner method, `Inflate`, and hence defaulted to `0.0`, regardless of its value, thereby leading to an incorrect z-coordinate in the returned `SE3Pose`.

### Deprecations

Please see the [General Release Notes](https://dev.bostondynamics.com/docs/release_notes) for a description of the API deprecations included in release 4.1.0.

### Spot Sample Code

#### Updated

- Updated the [basic_robot_command.cpp](../cpp/examples/basic_robot_command/basic_robot_command.cpp) example to use the [RobotCommandFeedback](../protos/bosdyn/api/robot_command_service.proto#robotcommandfeedback) RPC instead of sleeping for an arbitrary amount of time.

## Spot C++ SDK version 4.0.3 BETA

No changes from 4.0.2.

## Spot C++ SDK version 4.0.2 BETA

Please see the [General Release Notes](https://dev.bostondynamics.com/docs/release_notes) for a description of the API changes included in release 4.0.2.

### New Features

#### Joint Control

The Joint Control API allows for low-level control of the robot's joints. Note that this API is experimental and license-limited; the robot must have a Joint Level Control license installed in order for this API to be used. Please see the [documentation](concepts/joint_control/README.md) for more information and [supplemental](concepts/joint_control/supplemental_data.md) robot information which may be useful for development or simulation of the robot.

### Bug Fixes and Improvements

#### API

Please see the [General Release Notes](https://dev.bostondynamics.com/docs/release_notes) for a description of the API changes included in release 4.0.2.

#### SDK

- Added [RobotCommandStreamingClient](../cpp/bosdyn/client/robot_command/robot_command_streaming_client.cpp) client to support corresponding proto additions

- Added [RobotStateStreamingClient](../cpp/bosdyn/client/robot_state/robot_state_streaming_client.cpp) client to support corresponding proto additions

- Added `JointCommand` method to [robot_command_builder.cpp](../cpp/bosdyn/client/robot_command/robot_command_builder.cpp)

- Added helpers for the Joint Control API to [joint_api_helper.cpp](../cpp/examples/joint_control/joint_api_helper.cpp).

- Added `BlockUntilStandComplete` and `BlockUntilSE2TrajectoryComplete` functions to [robot_command_helpers.cpp](../cpp/bosdyn/client/robot_command/robot_command_helpers.cpp)

### Deprecations

- Deprecated `ValidateRequestHeaderAndRespond` method in [common_header_handling.h](../cpp/bosdyn/common/common_header_handling.h). Please use the other overload of `ValidateRequestHeaderAndRespond` going forward.

- Deprecated `BlockUntilArmArrives` and `BlockUntilGripperArrives` methods in [robot_command_helpers.h](../cpp/bosdyn/client/robot_command/robot_command_helpers.h). Please use the other overloads of `BlockUntilArmArrives` and `BlockUntilGripperArrives` going forward.

### Spot Sample Code

#### New

- [Joint Control: Robot Squat](../cpp/examples/joint_control/README.md#armless-robot-squat): An example that utilizes the joint control API to move the robot for a robot without an arm attached.

- [Joint Control: Wiggle Arm](../cpp/examples/joint_control/README.md#arm-wiggle): An example that utilizes the joint control API to move the robot for a robot with an arm attached.

## Spot C++ SDK version 4.0.1 BETA

Please see the [General Release Notes](https://dev.bostondynamics.com/docs/release_notes) for a description of the API changes included in release 4.0.1.

In addition to that, there are no changes in 4.0.1 except the following:

- The VERSION field in the project name has been updated to 4.0.1 from 4.0.0

## Spot C++ SDK version 4.0.0 BETA

### Breaking Changes

The following fields and services have been **removed**.

- `LogAnnotationService`
- Auth application token.
- Robot commands: non-synchronized mobility commands. Top-level feedback messages.
- Graph Nav map edge annotations: `vel_limit`, `ground_mu_hint`, `grated_floor`
- SpotCheck feedback: `foot_height_results` and `leg_pair_results`
- The choreography proto libraries have been deprecated and removed. The protos included in `libbosdyn_choreography_protos.so` and `libbosdyn_choreography_protos_static.a` will now be included in `libbosdyn_api.so` and `libbosdyn_api_static.a`.

### Bug Fixes and Improvements

#### API

Please see the [General Release Notes](https://dev.bostondynamics.com/docs/release_notes) for a description of the API changes included in release 4.0.0.

#### SDK

The C++ standard in the CMake file has been changed from cxx_std_14 to cxx_std_17.

Clients are now configured with a default 5s keep-alive time, which triggers a faster reconnect with the service, when the network connection goes down.

Lease update change: ignore failed old leases in the case the wallet contains the new lease.

Data Acquisition Client

- Added `GetLiveData` method to allow payloads to publish live data to the robot that is displayed on the tablet and Orbit during teleoperation

Data Acquisition Store Client

- Added `QueryStoredCaptures` method to enable retrieval of data from the robot using gRPC instead of the `/v1/data-buffer/daq-data/` endpoint
- Added `QueryMaxCaptureId` method, which returns the the `max_capture_id` corresponding to the greatest capture ID on the robot; this can be used to exclude preexisting data on the robot from retrieval by setting the `captures_from_id` field in the `QueryParameters` message accordingly.

Mission Client

- Added `GetInfoAsChunks` method, which enables the retrieval of static information describing the mission for missions whose size exceeds the maximum message size.

Power Client

- Added `ResetSafetyStop` method, which enables the primary and redundant safety stops on a Safety-Related Stopping Function (SRSF) configured robot. Robots equipped with this feature will be listed as SRSF "Enabled" under the hardware information section found in the "About" page on the robot's admin console.
- Added a set of helper functions in `power_client_helper.h` to be used with a power client.

SpotCam Compositor Client

- Now sets both the `coords` (deprecated in v3.3) and `meter` fields for backwards compatibility purposes in the corresponding methods.

Math & Frame Helpers

- The `ExternalStringToInternalString` and `InternalStringToExternalString` methods now return `std::nullopt` if the frame name does not exist.
- The kinematic_state's transforms_snapshot now uses "arm0.link_wr1" instead of "link_wr1" for the name of the frame attached to the SpotArm's wr1 link. This is (1) the name used in the URDF description of the robot and (2) the name used in the image service snapshots. We will continue to publish the kinematic_state's snapshot with the deprecated name in the 4.0 release, but it will be removed in a future release.
- An enum class called `RobotSpecies` has been added.
- Two frame names have been added, namely `ecef` (Earth-centered, Earth-fixed) and `enu` (East-North-Up), that are used for localization with respect to the Earth.
- Renamed `find_tree_root` method as `FindTreeRoot`
- Added a helper function `ReparentFrame`

Autowalk Player

- Sets `navigation_id` instead of `root_id` in the `SetNodeMap` method; the latter is deprecated as of v4.0

### Deprecations

Please see the [General Release Notes](https://dev.bostondynamics.com/docs/release_notes) for a description of the API deprecations included in release 4.0.0.

### Dependencies

### Known Issues

### Sample Code

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

Please see the [General Release Notes](https://dev.bostondynamics.com/docs/release_notes) for a description of the API changes included in release 3.3.0.

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

Please see the [General Release Notes](https://dev.bostondynamics.com/docs/release_notes) for a description of the API deprecations included in release 3.3.0.

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

Enables API clients to specify high level autonomous behaviors for Spot using an easily editable format. The autowalk format is a list of actions and their associated locations. Using this service, users can program the robot to “go to location A, perform action A, go to location B, perform action B, etc.”. The autowalk service compiles the autowalk into a behavior tree that can then be uploaded to the robot and played using the mission service. Previously this feature was only available on the tablet, but now it is a service for all client applications.

#### Graph Nav – Area Callbacks

Enables users to register a callback that is called in certain areas of the map during navigation. These “Area Callbacks” can instruct the robot to wait until the area is safe to cross (such as a crosswalk), take control of the robot and perform an action (such as opening a door), or perform a background action while in a certain area of the map (such as flashing lights or playing sounds).
This enables integration with the [Graph Nav](concepts/autonomy/graphnav_service.md) navigation system to extend its capabilities in terms of safety and new actions while navigating. See the [Area Callback](concepts/autonomy/area_callbacks.md) documentation for more details.

### Bug Fixes and Improvements

#### API

Please see the [General Release Notes](https://dev.bostondynamics.com/docs/release_notes) for a description of the API changes included in release 3.2.0.

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

Please see the [General Release Notes](https://dev.bostondynamics.com/docs/release_notes) for a description of the API deprecations included in release 3.2.0.

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
