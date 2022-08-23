/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include <CLI/CLI.hpp>

#include "autowalk_player.hpp"

int main(int argc, char** argv) {
    // Initialize variables
    std::filesystem::path walk_folder;
    std::chrono::seconds pause_duration;
    bool fail_on_mission_question;

    // Parse command line arguements
    CLI::App parser{"QueryAutowalkStatus"};
    ::bosdyn::client::CommonCLIArgs common_args;
    ::bosdyn::client::AddCommonArguments(parser, common_args);
    parser.add_option("--autowalk_mission", walk_folder, "Autowalk mission folder")->required();
    parser
        .add_option("--mission_pause_duration", pause_duration, "Mission client pause_duration (s)")
        ->default_val(3);
    parser
        .add_option("--fail_on_mission_question", fail_on_mission_question,
                    "If mission question(s) arise, script will print question(s) then quit")
        ->default_val(false);
    CLI11_PARSE(parser, argc, argv);

    std::cout << walk_folder << std::endl;

    // Initialize autowalk player
    AutowalkPlayer autowalk_player;
    auto init_status = autowalk_player.Init(common_args);
    if (!init_status) {
        std::cerr << init_status.DebugString() << std::endl;
        return 1;
    }

    // Upload graph nav map
    auto upload_status = autowalk_player.UploadGraphAndSnapshots(walk_folder);
    if (!upload_status) {
        std::cerr << upload_status.DebugString() << std::endl;
        return 1;
    }
    // Load autowalk
    auto load_status = autowalk_player.LoadAutowalk(walk_folder);
    if (!load_status) {
        std::cerr << load_status.DebugString() << std::endl;
        return 1;
    }

    // Play autowalk
    auto play_status = autowalk_player.PlayAutowalk(pause_duration, fail_on_mission_question);
    if (!play_status) {
        std::cerr << play_status.DebugString() << std::endl;
        return 1;
    }

    std::cout << "------Query Autowalk Status Application Worked as Expected!" << std::endl;
    return 0;
}