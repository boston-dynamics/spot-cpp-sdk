<!--
Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.

Downloading, reproducing, distributing or otherwise using the SDK Software
is subject to the terms and conditions of the Boston Dynamics Software
Development Kit License (20191101-BDSDK-SL).
-->

# Spot C++ SDK Release Notes

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
