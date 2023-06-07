<!--
Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.

Downloading, reproducing, distributing or otherwise using the SDK Software
is subject to the terms and conditions of the Boston Dynamics Software
Development Kit License (20191101-BDSDK-SL).
-->

# Simple Example for SpotCAM PTZ Control

This example program points the SpotCAM PTZ to a specific pan/tilt/zoom that the user specifies as command-line arguments.

## Run the Example
To run the example, set BOSDYN_CLIENT_USERNAME and BOSDYN_CLIENT_PASSWORD environment variables with the robot credentials and then run:
```
./ptz_example {ROBOT_IP} --ptz-choice digi --pan 100 --tilt 0 --zoom 5
```

The command-line arguments accepted are:
- `--ptz-choice`: PTZ to set position of.
- `--pan`: Pan position of the PTZ.
- `--tilt`: Tilt position of the PTZ.
- `--zoom`: Zoom position of the PTZ.
