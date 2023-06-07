/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "autowalk_player.hpp"
#include "bosdyn/client/service_client/rpc_parameters.h"

AutowalkPlayer::AutowalkPlayer() {}

AutowalkPlayer::~AutowalkPlayer() {
    if (m_lease_keep_alive) {
        auto return_lease_status = ReturnBodyLease();
        if (!return_lease_status) std::cerr << return_lease_status.DebugString() << std::endl;
    }
}

::bosdyn::common::Status AutowalkPlayer::Init(::bosdyn::client::CommonCLIArgs& args) {
    auto robot_setup_status = RobotSetup(args);
    if (!robot_setup_status) return robot_setup_status.Chain("Failed to setup robot.");

    auto set_clients_status = SetServiceClients();
    if (!set_clients_status)
        return set_clients_status.Chain("Failed to get necessary service clients.");

    auto acquire_lease_status = AcquireBodyLease();
    if (!acquire_lease_status) return acquire_lease_status.Chain("Failed to acquire body lease.");

    return ::bosdyn::common::Status(::bosdyn::client::SDKErrorCode::Success);
}

::bosdyn::common::Status AutowalkPlayer::RobotSetup(::bosdyn::client::CommonCLIArgs& args) {
    // Create a Client SDK object.
    std::unique_ptr<::bosdyn::client::ClientSdk> client_sdk =
        ::bosdyn::client::CreateStandardSDK("load_mission");
    std::cout << "------Created SDK" << std::endl;

    // Create a robot instance. A robot instance represents a single user on a single robot.
    auto robot_result = client_sdk->CreateRobot(args.hostname);
    if (!robot_result) return robot_result.status.Chain("Could not create robot.");
    m_robot = robot_result.move();
    std::cout << "------Created Robot" << std::endl;

    // Set the username and password to use. If you do not already have an account on a robot, ask
    // your administrator to add an account.
    auto robot_status = m_robot->Authenticate(args.username, args.password);
    if (!robot_status) return robot_status.Chain("Could not authenticate with robot.");
    std::cout << "------Authenticated with Robot" << std::endl;

    return ::bosdyn::common::Status(::bosdyn::client::SDKErrorCode::Success);
}

::bosdyn::common::Status AutowalkPlayer::SetServiceClients() {
    // Create a lease client
    auto lease_client_result = m_robot->EnsureServiceClient<::bosdyn::client::LeaseClient>();
    if (!lease_client_result)
        return lease_client_result.status.Chain("Could not create lease client.");
    m_lease_client = lease_client_result.move();
    std::cout << "------Created Lease Client" << std::endl;

    // Create a mission client
    auto mission_client_result =
        m_robot->EnsureServiceClient<::bosdyn::client::mission::MissionClient>();
    if (!mission_client_result)
        return mission_client_result.status.Chain("Could not create mission client.");
    m_mission_client = mission_client_result.move();

    // Create a autowalk client
    auto autowalk_client_result =
        m_robot->EnsureServiceClient<::bosdyn::client::autowalk::AutowalkClient>();
    if (!autowalk_client_result)
        return autowalk_client_result.status.Chain("Could not create autowalk client.");
    m_autowalk_client = autowalk_client_result.move();
    std::cout << "------Created Autowalk Client" << std::endl;

    // Create a graph nav client
    auto graph_nav_client_result = m_robot->EnsureServiceClient<::bosdyn::client::GraphNavClient>();
    if (!graph_nav_client_result)
        return graph_nav_client_result.status.Chain("Could not create graph nav client.");
    m_graph_nav_client = graph_nav_client_result.move();
    std::cout << "------Created Graph Nav Client" << std::endl;

    return ::bosdyn::common::Status(::bosdyn::client::SDKErrorCode::Success);
}

::bosdyn::common::Status AutowalkPlayer::AcquireBodyLease() {
    // Acquire body lease
    auto lease_result = m_lease_client->AcquireLease(::bosdyn::client::kBodyResource);
    if (!lease_result) return lease_result.status.Chain("Could not acquire body lease.");

    // Asynchronously retain body lease
    m_lease_keep_alive = std::make_unique<::bosdyn::client::LeaseKeepAlive>(m_lease_client);
    std::cout << "------Acquired Body Lease" << std::endl;

    return ::bosdyn::common::Status(::bosdyn::client::SDKErrorCode::Success);
}

::bosdyn::common::Status AutowalkPlayer::ReturnBodyLease() {
    // Stop retain_lease rpc calls
    m_lease_keep_alive->StopKeepAliveThread();

    // Get body lease proto
    auto lease_wallet = m_robot->GetWallet();
    auto body_lease_result = lease_wallet->GetOwnedLeaseProto("body");
    if (!body_lease_result)
        return body_lease_result.status.Chain("Could not get owned body lease proto.");
    auto body_lease = body_lease_result.move();

    // Return body lease
    ::bosdyn::api::ReturnLeaseRequest return_lease_request;
    *return_lease_request.mutable_lease() = body_lease;
    auto return_lease_result = m_lease_client->ReturnLease(return_lease_request);
    if (!return_lease_result)
        return return_lease_result.status.Chain("Failed to return body lease.");
    std::cout << "------Returned Body Lease" << std::endl;

    return ::bosdyn::common::Status(::bosdyn::client::SDKErrorCode::Success);
}

void AutowalkPlayer::SetNodeMap(
    const ::bosdyn::api::autowalk::LoadAutowalkResponse& response,
    const google::protobuf::RepeatedPtrField<bosdyn::api::autowalk::Element>& elements) {
    // Lambda function that adds the m_node_map
    auto AddToMap = [this](const int node_id, const std::string& debug_string) {
        std::pair<std::string, ::bosdyn::api::mission::Result> val(
            debug_string, ::bosdyn::api::mission::Result::RESULT_UNKNOWN);
        m_node_map[node_id] = val;
    };

    for (int i = 0; i < response.element_identifiers_size(); i++) {
        // Get element name
        std::string element_name;
        if (elements[i].name().empty()) {
            element_name = "[no name]";
        } else {
            element_name = elements[i].name();
        }

        // Get element identifier
        const auto& element_identifier = response.element_identifiers(i);

        // Map element identifier node ids to debug string and status
        const std::string debug_string =
            "Element " + std::to_string(i) + ": " + element_name + "\n";
        if (element_identifier.root_id().user_data_id().size() > 0) {
            AddToMap(element_identifier.root_id().node_id(), debug_string + "Root Status: ");
        }
        if (element_identifier.action_id().user_data_id().size() > 0) {
            AddToMap(element_identifier.action_id().node_id(), debug_string + "Action Status: ");
        }
    }

    // Map docking node id to debug string and status
    if (response.has_docking_node()) {
        AddToMap(response.docking_node().node_id(), "Docking Status: ");
    }

    // Map loop node id to debug string and status
    AddToMap(m_loop_node_id, "Number of walk cycles completed successfully: ");
}

::bosdyn::common::Status AutowalkPlayer::UploadGraphAndSnapshots(
    const std::filesystem::path& walk_folder) {
    // Read graph from file
    const std::filesystem::path graph_path = walk_folder / ::bosdyn::common::kGraphFile;
    ::bosdyn::api::graph_nav::Graph graph;
    auto read_graph_status = ReadProtoFromFile(graph_path, &graph);
    if (!read_graph_status) return read_graph_status.Chain("Failed to read graph.");

    // Upload graph to robot
    bool generate_new_anchoring = (graph.anchoring().anchors().size() == 0) ? true : false;
    ::bosdyn::api::graph_nav::UploadGraphRequest upload_graph_request;
    *upload_graph_request.mutable_graph() = graph;
    upload_graph_request.set_generate_new_anchoring(generate_new_anchoring);
    auto upload_graph_result = m_graph_nav_client->UploadGraph(upload_graph_request);
    if (!upload_graph_result)
        return upload_graph_result.status.Chain("Failed to upload graph to robot.");
    auto upload_graph_response = upload_graph_result.move();

    // Upload unknown waypoints to robot
    for (const auto& waypoint_id : upload_graph_response.unknown_waypoint_snapshot_ids()) {
        // Read waypoint snapshot from file
        const std::filesystem::path waypoint_filename =
            walk_folder / ::bosdyn::common::kWaypointSnapshotDir / waypoint_id;
        ::bosdyn::api::graph_nav::WaypointSnapshot waypoint_snapshot;
        auto read_waypoint_snapshot_status =
            ReadProtoFromFile(waypoint_filename, &waypoint_snapshot);
        if (!read_waypoint_snapshot_status)
            return read_waypoint_snapshot_status.Chain("Failed to read waypoint snapshot.");

        // Upload waypoint snapshot to robot
        auto upload_waypoint_snapshot_result =
            m_graph_nav_client->UploadWaypointSnapshot(waypoint_snapshot);
        if (!upload_waypoint_snapshot_result) {
            std::string err_string =
                "Failed to upload waypoint snapshot to robot. ID: " + waypoint_id;
            return upload_waypoint_snapshot_result.status.Chain(err_string);
        }
    }

    // Upload unknown edges to robot
    for (const auto& edge_id : upload_graph_response.unknown_edge_snapshot_ids()) {
        // Read edge snapshot from file
        const std::filesystem::path edge_filename =
            walk_folder / ::bosdyn::common::kEdgeSnapshotDir / edge_id;
        ::bosdyn::api::graph_nav::EdgeSnapshot edge_snapshot;
        auto read_edge_snapshot_status = ReadProtoFromFile(edge_filename, &edge_snapshot);
        if (!read_edge_snapshot_status)
            return read_edge_snapshot_status.Chain("Failed to read edge snapshot.");

        // Upload edge snapshot to robot
        auto upload_edge_snapshot_result = m_graph_nav_client->UploadEdgeSnapshot(edge_snapshot);
        if (!upload_edge_snapshot_result) {
            std::string err_string = "Failed to upload edge snapshot to robot. ID: " + edge_id;
            return upload_edge_snapshot_result.status.Chain(err_string);
        }
    }

    std::cout << "------Loaded Graph and Snapshots to Robot" << std::endl;

    return ::bosdyn::common::Status(::bosdyn::client::SDKErrorCode::Success);
}

::bosdyn::common::Status AutowalkPlayer::LoadAutowalk(const std::filesystem::path& walk_path) {
    // Read walk from file
    ::bosdyn::api::autowalk::Walk walk;
    auto read_walk_status = ReadProtoFromFile(walk_path, &walk);
    if (!read_walk_status) return read_walk_status.Chain("Failed to read walk.");

    // Load autowalk
    ::bosdyn::api::autowalk::LoadAutowalkRequest load_autowalk_request;
    *load_autowalk_request.mutable_walk() = walk;
    auto load_autowalk_result = m_autowalk_client->LoadAutowalk(load_autowalk_request);
    if (!load_autowalk_result)
        return load_autowalk_result.status.Chain("Could not load autowalk to robot");
    auto load_autowalk_response = load_autowalk_result.move();
    std::cout << "------Loaded Autowalk to Robot" << std::endl;

    // Store loop node id
    m_loop_node_id = load_autowalk_response.loop_node().node_id();

    // Map node id to a string with debug info and node status
    SetNodeMap(load_autowalk_response, load_autowalk_request.walk().elements());

    return ::bosdyn::common::Status(::bosdyn::client::SDKErrorCode::Success);
};

::bosdyn::common::Status AutowalkPlayer::PlayAutowalk(const std::chrono::seconds pause_duration,
                                                      const bool fail_on_mission_question) {
    // Verify the robot is not estopped and that an external application has registered and holds
    // an estop endpoint.
    auto estop_result = m_robot->IsEstopped();
    if (!estop_result) return estop_result.status.Chain("Could not check estop status.");
    if (estop_result.response) {
        std::string err_string =
            "Robot is estopped. Please use an external E-Stop client, such as the estop Python SDK "
            "example, to configure E-Stop.";
        return ::bosdyn::common::Status(::bosdyn::client::SDKErrorCode::GenericSDKError,
                                        err_string);
    }
    std::cout << "------E-Stop configured" << std::endl;

    // Start time sync
    auto time_sync_endpoint_result = m_robot->StartTimeSyncAndGetEndpoint();
    if (!time_sync_endpoint_result)
        return time_sync_endpoint_result.status.Chain("Could not sync time with robot");
    auto time_sync_endpoint = time_sync_endpoint_result.move();
    std::cout << "------Started Time Sync Thread" << std::endl << std::endl;

    // Initialize mission play settings
    ::bosdyn::api::mission::PlaySettings play_settings;
    play_settings.set_path_following_mode(
        ::bosdyn::api::graph_nav::Edge::Annotations::PathFollowingMode::
            Edge_Annotations_PathFollowingMode_PATH_MODE_DEFAULT);

    int last_counter = -1;
    bosdyn::api::mission::State mission_state;
    mission_state.set_status(bosdyn::api::mission::State_Status::State_Status_STATUS_NONE);
    do {
        // Initialize play request
        ::bosdyn::api::mission::PlayMissionRequest play_request;
        *play_request.mutable_settings() = play_settings;

        // Create local pause time then convert to robot time
        const std::chrono::time_point<std::chrono::system_clock> local_timepoint =
            std::chrono::system_clock::now() + pause_duration;
        *play_request.mutable_pause_time() =
            time_sync_endpoint->RobotTimestampFromLocal(local_timepoint);

        // Send play request
        auto play_result = m_mission_client->PlayMission(play_request);
        if (!play_result) {
            // Print error then try again
            if (play_result.status.code() == RetryableRPCCondition::Retryable) {
                std::cout << play_result.status.DebugString() << std::endl << std::endl;
                sleep(1);
                continue;
            } else
                return play_result.status;
        }

        // Initialize get state request
        ::bosdyn::api::mission::GetStateRequest get_state_request;

        // Get all ticks since last query
        get_state_request.set_history_lower_tick_bound(last_counter + 1);

        // Send get state request
        auto mission_state_result = m_mission_client->GetState(get_state_request);
        if (!mission_state_result)
            return mission_state_result.status.Chain("Could not get mission state");
        mission_state = mission_state_result.move().state();
        last_counter = mission_state.tick_counter();

        // Check for mission questions
        if (fail_on_mission_question && mission_state.questions_size() > 0) {
            ::bosdyn::common::Status question_status(
                ::bosdyn::client::SDKErrorCode::GenericSDKError);
            for (const auto& question : mission_state.questions()) {
                question_status = question_status.Chain(question.text());
            }
            return question_status.Chain("Failed due to the following mission question(s)");
        }

        // Query autowalk elements
        QueryAutowalkElements(mission_state.history());

        sleep(1);

    } while (mission_state.status() ==
                 ::bosdyn::api::mission::State_Status::State_Status_STATUS_NONE ||
             mission_state.status() ==
                 ::bosdyn::api::mission::State_Status::State_Status_STATUS_RUNNING);

    return ::bosdyn::common::Status(::bosdyn::client::SDKErrorCode::Success);
}

void AutowalkPlayer::QueryAutowalkElements(
    const google::protobuf::RepeatedPtrField<bosdyn::api::mission::State_NodeStatesAtTick>&
        node_histories) {
    // Loop through histories starting with oldest
    auto history = node_histories.rbegin();
    while (history != node_histories.rend()) {
        // Loop through state of each node in this tick history
        for (const auto& node_state : history->node_states()) {
            // Do nothing if node is not in the node map
            auto search_result = m_node_map.find(node_state.id());
            if (search_result == m_node_map.end()) continue;

            // Parse search result
            const int node_id = search_result->first;
            bosdyn::api::mission::Result& node_status = search_result->second.second;
            const std::string debug_string = search_result->second.first;

            // Do nothing if node status remained constant
            if (node_status == node_state.result()) continue;

            // Store new node state in node map
            node_status = node_state.result();

            // Check for loop node
            if (node_id == m_loop_node_id) {
                // Increment and print loop counter if walk cycle has completed
                // This information is especially useful for periodic/continuous walks
                if (node_status == bosdyn::api::mission::Result::RESULT_SUCCESS) {
                    m_loop_counter++;
                    std::cout << debug_string << m_loop_counter << std::endl << std::endl;
                }
                continue;
            }

            // If node is an element identifier or docking node and the status has changed, print
            // the new status
            std::cout << debug_string << ::bosdyn::api::mission::Result_Name(node_state.result())
                      << std::endl
                      << std::endl;
        }

        history++;
    }
}

template <typename ProtoType>
::bosdyn::common::Status ReadProtoFromFile(const std::filesystem::path& file, ProtoType* proto) {
    std::ifstream input_file(file);
    if (!input_file.is_open()) {
        std::string err_string = "Could not open the file - '" + std::string(file) + "'";
        return ::bosdyn::common::Status(::bosdyn::client::SDKErrorCode::GenericSDKError,
                                        err_string);
    }
    std::string proto_data((std::istreambuf_iterator<char>(input_file)),
                           std::istreambuf_iterator<char>());
    if (!proto->ParseFromString(proto_data)) {
        std::string err_string = "Failed to parse " + ProtoType::descriptor()->full_name() +
                                 " from file - '" + std::string(file) + "'";
        return ::bosdyn::common::Status(::bosdyn::client::SDKErrorCode::GenericSDKError,
                                        err_string);
    }
    return ::bosdyn::common::Status(::bosdyn::client::SDKErrorCode::Success);
}

::bosdyn::common::Status GetMissionName(const std::filesystem::path& walk_folder,
                                        std::string* mission_name) {
    // Try using the map name. By default the walk folder has the same name as the map.
    std::filesystem::path walk_path =
        walk_folder / ::bosdyn::common::kMissionsDir / walk_folder.filename();
    if (std::filesystem::exists(walk_path)) {
        *mission_name = walk_folder.filename();
        return ::bosdyn::common::Status(::bosdyn::client::SDKErrorCode::Success);
    }

    // Try using "autogenerated.walk". Prior to 3.3 mission were named "autogenerated.walk" by
    // default.
    walk_path = walk_folder / ::bosdyn::common::kMissionsDir / ::bosdyn::common::kAutowalkFile;
    if (std::filesystem::exists(walk_path)) {
        *mission_name = ::bosdyn::common::kAutowalkFile;
        return ::bosdyn::common::Status(::bosdyn::client::SDKErrorCode::Success);
    }

    return ::bosdyn::common::Status(::bosdyn::client::SDKErrorCode::GenericSDKError,
                                    "Failed to find mission file. Please specify the mission "
                                    "filename using the --mission_name command line argument.");
}
