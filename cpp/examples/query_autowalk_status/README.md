<!--
Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.

Downloading, reproducing, distributing or otherwise using the SDK Software
is subject to the terms and conditions of the Boston Dynamics Software
Development Kit License (20191101-BDSDK-SL).
-->

# Query Autowalk Status

This example program loads an autowalk and graph nav map to the robot from a file. It then queries the status of elements and actions in the walk as the autowalk mission plays.

## Run the Example
To run the example, set BOSDYN_CLIENT_USERNAME and BOSDYN_CLIENT_PASSWORD environment variables with the robot credentials and then run:
```
./query_autowalk_status {ROBOT_IP} --autowalk_mission={Path to Autowalk Mission Folder}
```
To see a list a full command line arguements run:
```
./query_autowalk_status --help
```