<!--
Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.

Downloading, reproducing, distributing or otherwise using the SDK Software
is subject to the terms and conditions of the Boston Dynamics Software
Development Kit License (20191101-BDSDK-SL).
-->

# Hello Spot

This example program is the introductory programming example for Spot.  It demonstrates how to initialize the SDK to talk to robot and how to get information from the robot services, such as robot id, robot state, list of services running on the robot, and the list of registered payloads. It also logs data to the DataBuffer service.

## Understanding Spot Programming
For your best learning experience, please use the [Quickstart Guide](../../../docs/cpp/quickstart.md) found in the SDK's docs/cpp directory.  That will help you get your C++ programming environment set up properly.  Then, specifically for Hello Spot, you should look at the [Understanding Spot Programming](../../../docs/python/understanding_spot_programming.md) file. This document walks you through all the commands found in this example!

## Run the Example
To run the example, set BOSDYN_CLIENT_USERNAME and BOSDYN_CLIENT_PASSWORD environment variables with the robot credentials and then run:
```
./hello_spot {ROBOT_IP}
```
