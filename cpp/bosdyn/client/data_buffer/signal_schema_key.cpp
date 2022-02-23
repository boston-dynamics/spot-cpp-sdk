/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "signal_schema_key.h"

#include <string>

namespace bosdyn {

namespace client {
namespace data_buffer {

namespace {
uint64_t djb2(const char* str, size_t sz) {
    uint64_t hash = 5381;
    for (size_t i = 0; i < sz; i++) {
        hash = ((hash << 5) + hash) + static_cast<int>(str[i]);
    }
    return hash;
}
}  // namespace

uint64_t SignalSchemaKey::ComputeHash() {
    const std::string repr = std::to_string(m_salt) + ClientName() + SerializedSchema();
    m_hash = djb2(repr.c_str(), repr.size());
    return m_hash;
}

uint64_t SignalSchemaKey::GenerateReplacementHash() {
    m_salt += 1;
    return ComputeHash();
}

}  // namespace data_buffer
}  // namespace client

}  // namespace bosdyn
