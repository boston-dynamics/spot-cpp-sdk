/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <unistd.h>
#include <filesystem>

#include "bosdyn/client/autowalk/autowalk_client.hpp"
#include "bosdyn/client/graph_nav/graph_nav_client.h"
#include "bosdyn/client/lease/lease_client.h"
#include "bosdyn/client/lease/lease_keepalive.h"
#include "bosdyn/client/mission/mission_client.h"
#include "bosdyn/client/sdk/client_sdk.h"
#include "bosdyn/client/util/cli_util.h"
#include "bosdyn/common/mission_filesystem.h"

class AutowalkPlayer {
 public:
    AutowalkPlayer();
    ~AutowalkPlayer();

    /**
     * Creates and authenticates robot, ensures are necessary services are registered, and acquires
     * body lease.
     *
     * @param args Hostname, username, and password of robot.
     *
     * @returns Status message that includes any failures that might have occured.
     */
    ::bosdyn::common::Status Init(::bosdyn::client::CommonCLIArgs& args);

    /**
     * Uploads graph, waypoint snapshots, and edge snapshots to robot.
     *
     * @param mission_folder File path to autowalk mission folder.
     *
     * @returns Status message that includes any failures that might have occured.
     */
    ::bosdyn::common::Status UploadGraphAndSnapshots(const std::filesystem::path& walk_folder);

    /**
     * Loads autowalk to the robot using the autowalk service.
     *
     * @param walk_path File path to walk file in walk folder.
     *
     * @returns Status message that includes any failures that might have occured.
     */
    ::bosdyn::common::Status LoadAutowalk(const std::filesystem::path& walk_path);

    /**
     * Plays the mission loaded on the robot.
     *
     * @param pause_duration Mission client timeout (s). Robot will pause mission execution if a new
     * play request is not received within this time frame.
     * @param fail_on_mission_question Script will terminate if mission question(s) arise. Questions
     * will print to console before script ends.
     *
     * @returns Status message that includes any failures that might have occured.
     */
    ::bosdyn::common::Status PlayAutowalk(const std::chrono::seconds pause_duration,
                                          const bool fail_on_mission_question);

 private:
    /**
     * Creates and authenticates a robot instance.
     *
     * @param args Hostname, username, and password of robot.
     *
     * @returns Status message that includes any failures that might have occured.
     */
    ::bosdyn::common::Status RobotSetup(::bosdyn::client::CommonCLIArgs& args);

    /**
     * Ensures are necessary services are registered.
     *
     * @returns Status message that includes any failures that might have occured.
     */
    ::bosdyn::common::Status SetServiceClients();

    /**
     * Acquires robot body lease.
     *
     * @returns Status message that includes any failures that might have occured.
     */
    ::bosdyn::common::Status AcquireBodyLease();

    /**
     * Returns robot body lease.
     *
     * @returns Status message that includes any failures that might have occured.
     */
    ::bosdyn::common::Status ReturnBodyLease();

    /**
     * Prints the status of walk critical nodes as the mission plays. Keeps count of walk cycles
     * successfully executed.
     *
     * @param node_histories List of node histories since the function was last called.
     */
    void QueryAutowalkElements(
        const google::protobuf::RepeatedPtrField<bosdyn::api::mission::State_NodeStatesAtTick>&
            node_histories);

    /**
     * Creates a map of walk critical nodes that correlates node id to node status and a debug
     * string.
     *
     * @param response Response from LoadAutowalk rpc call.
     * @param elements List of elements in walk.
     */
    void SetNodeMap(
        const ::bosdyn::api::autowalk::LoadAutowalkResponse& response,
        const google::protobuf::RepeatedPtrField<bosdyn::api::autowalk::Element>& elements);

    // Robot instance
    std::unique_ptr<bosdyn::client::Robot> m_robot;

    // Lease service client
    ::bosdyn::client::LeaseClient* m_lease_client = nullptr;

    // Mission service client
    ::bosdyn::client::mission::MissionClient* m_mission_client = nullptr;

    // Autowalk service client
    ::bosdyn::client::autowalk::AutowalkClient* m_autowalk_client = nullptr;

    // Graph Nav service client
    ::bosdyn::client::GraphNavClient* m_graph_nav_client = nullptr;

    // Pointer to object that asynchronously retains the body lease
    std::unique_ptr<::bosdyn::client::LeaseKeepAlive> m_lease_keep_alive;

    // Map that correlates node id to node status and a debug string
    std::unordered_map<int, std::pair<std::string, ::bosdyn::api::mission::Result>> m_node_map;

    // Node id of the loop node that repeats in continuous and periodic missions.
    int m_loop_node_id;

    // Number of times main mission sequence has completed.
    int m_loop_counter;
};

/**
 * Read a protobuf message from a file.
 *
 * @param file Path to file.
 * @param proto Address to store the protobuf.
 *
 * @returns Status message that includes any failures that might have occured.
 */
template <typename ProtoType>
::bosdyn::common::Status ReadProtoFromFile(const std::filesystem::path& file, ProtoType* proto);

/**
 * Attempts to find the walk file from the walk folder.
 *
 * @param walk_folder File path to autowalk mission folder.
 *
 * @param mission_name If a valid mission name is found, it will be stored at this address.
 *
 * @returns Status message that includes any failures that might have occured.
 */
::bosdyn::common::Status GetMissionName(const std::filesystem::path& walk_folder,
                                        std::string* mission_name);
