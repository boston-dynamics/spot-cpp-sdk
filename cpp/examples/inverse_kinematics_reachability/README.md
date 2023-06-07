<!--
Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.

Downloading, reproducing, distributing or otherwise using the SDK Software
is subject to the terms and conditions of the Boston Dynamics Software
Development Kit License (20191101-BDSDK-SL).
-->

# INVERSE KINEMATICS REACHABILITY

This example program shows how to use the Inverse Kinematics Service API to make reachability
queries for random arm poses.

The example:

- Defines desired tool poses randomly distributed across a rectangle in front of the robot, and a small distance above the ground.
- Defines a tool frame at the tip of the lower jaw, oriented such that the opening of the
  gripper will point down when the tool is at one of the desired tool poses.
- Formulates an InverseKinematicsRequest for each desired tool pose, such that a solution
  will provide a configuration that places the tool frame at the desired tool pose, while keeping
  the feet in their current positions.
- Checks the results returned by the IK service:
  - For desired tool poses that the IK service deemed reachable, it issues an arm Cartesian move
    command with the desired body pose set to the one provided by IK.
  - For desired tool poses that the IK service deemed un-reachable, it issues an arm Cartesian
    move with `enable_hip_height_assist` and `enable_body_yaw_assist` both set to `true`.
- Prints the true and false positive/negative results.

## Run the Example

To run the example, set BOSDYN_CLIENT_USERNAME and BOSDYN_CLIENT_PASSWORD environment variables with the robot credentials and then run:

```
./inverse_kinematics_reachability {ROBOT_IP} --num_poses {NUM_DESIRED_POSES}
```
