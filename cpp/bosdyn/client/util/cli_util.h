/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include "bosdyn/client/robot/robot.h"

#include <CLI/CLI.hpp>

namespace bosdyn {

namespace client {

struct CommonCLIArgs {
    std::string hostname;
    std::string username;
    std::string password;
};

/**
 * Add base command-line arguments.
 *
 * @param parser(CLI::App): CLI11 App parser where to add the options.
 * @param hostname(std::string): Hostname variable mapped to CLI option
 */
void AddBaseArguments(CLI::App& parser, std::string& hostname);

/**
 * Add common command-line arguments.
 *
 * @param parser(CLI::App): CLI11 App parser where to add the options.
 * @param args(bosdyn::client::CommonCLIArgs): Variables mapped to CLI11 options.
 */
void AddCommonArguments(CLI::App& parser, CommonCLIArgs& args);


}  // namespace client

}  // namespace bosdyn
