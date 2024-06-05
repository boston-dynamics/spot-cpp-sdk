/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "joint_api_helper.hpp"

template <int N_DOFS>
void JointAPIInterface<N_DOFS>::activate(
    ::bosdyn::client::RobotCommandClient* robot_command_client) {
    // Function to activate full body joint control through RobotCommand
    // Wait for streaming to start
    while (!m_started_streaming) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        if (m_should_stop) {
            std::cerr << "should_stop is activated in activate process" << std::endl;
            return;
        }
    }

    // Activate joint control mode
    std::cout << "------Activating joint control" << std::endl;
    bosdyn::api::RobotCommand joint_command = ::bosdyn::client::JointCommand();
    auto joint_res = robot_command_client->RobotCommand(joint_command);
    if (!joint_res.status) {
        std::cerr << "Failed to activate the joint control mode: " << joint_res.status.DebugString()
                  << std::endl;
        m_should_stop = true;
        return;
    }
}

template <int N_DOFS>
void JointAPIInterface<N_DOFS>::handle_state_streaming(
    ::bosdyn::client::RobotStateStreamingClient* robot_state_stream_client) {
    while (!m_should_stop) {
        // Get robot state stream
        auto robot_state_stream = robot_state_stream_client->GetRobotStateStream();
        if (!robot_state_stream) {
            std::cout << "Failed to get the robot stream state: "
                      << robot_state_stream.status.DebugString() << std::endl;
            continue;
        }
        m_latest_state_stream_response = std::move(robot_state_stream.response);

        // The below is the logic to measure round trip time of the command by comparing
        // user_command_key from cmd_history
        auto receive_time = ::bosdyn::common::NowNsec();
        int user_command_key = m_latest_state_stream_response.last_command().user_command_key();
        if (user_command_key != 0 && m_cmd_history.find(user_command_key) != m_cmd_history.end()) {
            auto sent_time = m_cmd_history[user_command_key];
            double roundtrip_ms = (receive_time - sent_time) * 0.001 * 0.001;
            std::cout << "Roundtrip : " << roundtrip_ms << std::endl;
        } else {
            std::cout << "No key : " << user_command_key << std::endl;
        }
    }
}

template <int N_DOFS>
::bosdyn::common::Status JointAPIInterface<N_DOFS>::generate_joint_pos_interp_commands(
    ::bosdyn::client::RobotCommandStreamingClient* robot_command_stream_client,
    ::bosdyn::client::TimeSyncThread* time_sync_thread,
    std::vector<std::array<float, N_DOFS>> cmd_poses, std::array<float, N_DOFS> cmd_loads,
    int64_t duration_ns, std::array<float, N_DOFS> k_q_p, std::array<float, N_DOFS> k_qd_p) {
    std::array<float, N_DOFS> pos_cmd{0};
    std::array<float, N_DOFS> vel_cmd{0};
    std::array<float, N_DOFS> load_cmd{0};

    // Initalize the size of command
    ::bosdyn::api::JointControlStreamRequest request;
    ::bosdyn::client::SetRequestHeader("streaming_example_client", &request);

    auto* joint_cmd = request.mutable_joint_command();
    joint_cmd->mutable_gains()->mutable_k_q_p()->Reserve(N_DOFS);
    joint_cmd->mutable_gains()->mutable_k_qd_p()->Reserve(N_DOFS);
    joint_cmd->mutable_position()->Reserve(N_DOFS);
    joint_cmd->mutable_velocity()->Reserve(N_DOFS);
    joint_cmd->mutable_load()->Reserve(N_DOFS);

    for (int i = 0; i < N_DOFS; i++) {
        joint_cmd->mutable_gains()->mutable_k_q_p()->Add();
        joint_cmd->mutable_gains()->mutable_k_qd_p()->Add();
        joint_cmd->mutable_position()->Add();
        joint_cmd->mutable_velocity()->Add();
        joint_cmd->mutable_load()->Add();
    }

    // Generate joint plan which is linear interpolation planning for each leg joints.
    std::array<LinearInterpolator, N_DOFS> joints_to_control;

    // Joint command parameters in [ns]
    // Send the robot command in 333 Hz.
    int64_t dt = 3 * 1e6;  // 3 ms

    // Count the number of command sent
    int count = 0;

    // Now loop through cmd_poses
    for (int iter = 0; iter < cmd_poses.size() - 1; iter++) {
        bool new_command = true;
        auto starting_time_ns = ::bosdyn::common::NowNsec();
        auto previous_dt_time_ns = starting_time_ns;

        while (::bosdyn::common::NowNsec() < (starting_time_ns + duration_ns)) {
            count += 1;

            if (m_should_stop) {
                std::cerr << "should_stop is activated in generate command process" << std::endl;
                return {::bosdyn::client::SDKErrorCode::GenericSDKError};
            }

            // Want to send next command no sooner than `dt` after the previous one
            auto now = ::bosdyn::common::NowNsec();
            auto this_dt_time_d_ns = std::max(previous_dt_time_ns + dt, now);
            auto sleep_time_ns = std::max(0L, this_dt_time_d_ns - now);

            // Sleep in [us]
            usleep(sleep_time_ns * 1e-3);

            // Want to know time past since the starting_time
            auto this_dt_time_actual_ns = ::bosdyn::common::NowNsec();
            auto plan_time_ns = this_dt_time_actual_ns - starting_time_ns;

            for (int joint_idx = 0; joint_idx < N_DOFS; joint_idx++) {
                if (new_command) {
                    // Set up joint planner
                    joints_to_control[joint_idx] = LinearInterpolator(
                        duration_ns, cmd_poses[iter][joint_idx], cmd_poses[iter + 1][joint_idx]);
                }

                // Execute plan
                auto cmd_des = joints_to_control[joint_idx].calculate_command(plan_time_ns);
                pos_cmd[joint_idx] = cmd_des.pos_des;
                vel_cmd[joint_idx] = cmd_des.vel_des;
                load_cmd[joint_idx] = cmd_loads[joint_idx];

                // Fill in gains the first dt
                if (count == 1) {
                    joint_cmd->mutable_gains()->set_k_q_p(joint_idx, k_q_p[joint_idx]);
                    joint_cmd->mutable_gains()->set_k_qd_p(joint_idx, k_qd_p[joint_idx]);
                }
                joint_cmd->set_position(joint_idx, pos_cmd[joint_idx]);
                joint_cmd->set_velocity(joint_idx, vel_cmd[joint_idx]);
                joint_cmd->set_load(joint_idx, load_cmd[joint_idx]);
            }
            new_command = false;

            auto time_point_local = ::bosdyn::common::TimePoint(
                std::chrono::system_clock::now() + std::chrono::nanoseconds(100 * 1000 * 1000));
            ::bosdyn::common::RobotTimeConverter converter =
                time_sync_thread->GetEndpoint()->GetRobotTimeConverter();
            joint_cmd->mutable_end_time()->CopyFrom(
                converter.RobotTimestampFromLocal(time_point_local));

            // Let it extrapolate the command a little
            joint_cmd->mutable_extrapolation_duration()->CopyFrom(
                google::protobuf::util::TimeUtil::NanosecondsToDuration(5 * 1e6));

            // Set user key for latency tracking
            joint_cmd->set_user_command_key(count);

            // Track the actual send time
            m_cmd_history[count] = ::bosdyn::common::NowNsec();

            // Send joint stream command
            auto joint_control_stream = robot_command_stream_client->JointControlStream(request);
            if (!joint_control_stream) {
                std::cerr << "Failed to send robot command"
                          << joint_control_stream.status.DebugString() << std::endl;
                return joint_control_stream.status;
            }
            m_started_streaming = true;
            previous_dt_time_ns = this_dt_time_d_ns;
        }
    }

    return {::bosdyn::client::SDKErrorCode::Success};
}

template class JointAPIInterface<12>;
template class JointAPIInterface<19>;