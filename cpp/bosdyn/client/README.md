<!--
Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.

Downloading, reproducing, distributing or otherwise using the SDK Software
is subject to the terms and conditions of the Boston Dynamics Software
Development Kit License (20191101-BDSDK-SL).
-->

# Client Classes

The bosdyn/client folder contains client interfaces for interacting with the Boston Dynamics Spot 
API. The client interfaces implement the Remote Procedure Calls (RPCs) in the 
[API protobuf definitions](https://dev.bostondynamics.com/protos/bosdyn/api/readme).

## Main Classes
The main classes contained in this folder are ClientSdk and Robot.
* **ClientSdk**: Class for settings typically common to a single developer and/or robot fleet.
* **Robot**: Class for settings common to one user's access to one robot. This is the main point 
of access to all client functionality.

For example, to list the robot image sources using the Image client, simply create an Sdk object, 
create a Robot object, authenticate, create the client and call the 
corresponding method, as shown below. It is that simple!
```
auto client_sdk = ::bosdyn::client::CreateStandardSDK("hello_spot");
auto robot = client_sdk->CreateRobot("10.0.0.3").move();
robot->Authenticate(username, password).IgnoreError();
auto image_client = robot->EnsureServiceClient<::bosdyn::client::ImageClient>().move();
auto result =  image_client->ListImageSources().move();
```

Clients that trigger robot movement also need to set up the TimeSync, Lease and E-Stop clients. Our 
[Python examples](https://github.com/boston-dynamics/spot-sdk/tree/master/python/examples) repository contains many tutorials on how to use the clients. The API for the Spot Python SDK is very similar to the API for the Spot C++ SDK, and most Python examples can be used as reference on how to implement C++ applications. The Spot C++ SDK contains examples as well, listed [here](../../examples/README.md).