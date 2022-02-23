/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

namespace bosdyn {

namespace client {

class TokenCache {
 public:
    TokenCache() = default;
    ~TokenCache() = default;

    std::string Read(const std::string& /*name*/) { return ""; }

    void clear(const std::string& /*name*/) {}

    void write(const std::string& /*name*/, const std::string& /*token*/) {}

    std::vector<std::string> match(const std::string& /*name*/) {
        return std::vector<std::string>();
    }
};

}  // namespace client

}  // namespace bosdyn
