/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include <google/protobuf/util/time_util.h>
#include <CLI/CLI.hpp>

#include "bosdyn/client/robot_command/robot_command_builder.h"
#include "bosdyn/client/robot_command/robot_command_client.h"
#include "bosdyn/client/robot_command/robot_command_helpers.h"
#include "bosdyn/client/robot_command/robot_command_streaming_client.h"
#include "bosdyn/client/robot_state/robot_state_streaming_client.h"
#include "bosdyn/client/sdk/client_sdk.h"
#include "bosdyn/client/service_client/client_header_handling.h"
#include "bosdyn/client/time_sync/time_sync_helpers.h"
#include "bosdyn/client/util/cli_util.h"

#include <unistd.h>
#include <chrono>
#include <cmath>
#include <thread>

// LinearInterpolator is a helper class to linearly interpolate between two joint positions over a
// fixed duration.
class LinearInterpolator {
 public:
    LinearInterpolator() = default;
    LinearInterpolator(int64_t duration, float init_pos, float target_pos) {
        m_duration = duration;
        m_init_pos = init_pos;
        m_target_pos = target_pos;
    }

    struct Return {
        float pos_des = 0.0f;
        float vel_des = 0.0f;

        Return() = delete;
    };
    Return calculate_command(int64_t time) {
        float pos = m_init_pos + (m_target_pos - m_init_pos) * static_cast<float>(time) /
                                     static_cast<float>(m_duration);
        float vel = (m_target_pos - m_init_pos) / m_duration;

        return Return{pos, vel};
    }

 private:
    int64_t m_duration = 0;  // In [ns]
    float m_init_pos = 0.0f;
    float m_target_pos = 0.0f;
};

// JointAPIInterface is a helper class to help to use joint control API. This class contains the way
// of activating joint control mode, getting lightweight robot state, and sending joint pos by
// interpolating commanded poses as an example.
template <int N_DOFS>
class JointAPIInterface {
 public:
    JointAPIInterface() = default;

    // should_stop is a flag to enable/disable sending/receiving command
    void set_should_stop(bool flag) { m_should_stop = flag; }

    // activate function is to activate joint control mode
    void activate(::bosdyn::client::RobotCommandClient* robot_command_client);

    // handle_state_streaming function is to get lightweight robot state streaming message.
    void handle_state_streaming(
        ::bosdyn::client::RobotStateStreamingClient* robot_state_stream_client);

    /**
     * generate_joint_pos_interp_commands functions is an exmaple function to send the joint command
     * by interpolating commanded poses
     *
     * robot_command_stream_client : client for RPC communication
     * time_sync_thread : to get synchronized time data from robot
     * cmd_poses : series of commanded joints poses
     * cmd_loads : commanded joint loads
     * duration_ns : desired duriation between the poses [ns]
     * k_q_p : proportional gain for position control
     * k_qd_p : proportional gain for velocity control
     */
    ::bosdyn::common::Status generate_joint_pos_interp_commands(
        ::bosdyn::client::RobotCommandStreamingClient* robot_command_stream_client,
        ::bosdyn::client::TimeSyncThread* time_sync_thread,
        std::vector<std::array<float, N_DOFS>> cmd_poses, std::array<float, N_DOFS> cmd_loads,
        int64_t duration_ns, std::array<float, N_DOFS> k_q_p, std::array<float, N_DOFS> k_qd_p);

    struct JointsState {
        std::array<float, N_DOFS> pos{0};
        std::array<float, N_DOFS> vel{0};
        std::array<float, N_DOFS> load{0};
    };

    // get_latest_joints_state function is to get a latest joint state
    JointsState get_latest_joints_state() {
        // Wait for the first data to cache. This should happend synchronously in normal stand
        // before joint control is activated.
        while (!m_latest_state_stream_response.has_joint_states()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // Cache current joint state
        for (int i = 0; i < N_DOFS; i++) {
            m_joints_state.pos[i] = m_latest_state_stream_response.joint_states().position(i);
            m_joints_state.vel[i] = m_latest_state_stream_response.joint_states().velocity(i);
            m_joints_state.load[i] = m_latest_state_stream_response.joint_states().load(i);
        }

        return m_joints_state;
    }

 private:
    ::bosdyn::api::RobotStateStreamResponse m_latest_state_stream_response;  // Latest robot state
                                                                             // stream response.
    bool m_should_stop = false;  // Triggered when the joint control mode should stop.
    std::unordered_map<int, int64_t> m_cmd_history;  // Store user_command_key in the purpose of
                                                     // checking round trip time of command.
    bool m_started_streaming = false;
    JointsState m_joints_state;
};