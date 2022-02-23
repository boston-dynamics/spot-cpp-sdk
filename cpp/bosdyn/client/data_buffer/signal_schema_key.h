/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <string>

namespace bosdyn {

namespace client {

namespace data_buffer {

class SignalSchemaKey {
 public:
    SignalSchemaKey(const std::string& client_name, const std::string& serialized_schema)
        : m_client_name(client_name), m_serialized_schema(serialized_schema) {
        ComputeHash();
    }
    ~SignalSchemaKey() = default;

    const std::string& ClientName() const { return m_client_name; }
    const std::string& SerializedSchema() const { return m_serialized_schema; }

    bool IsEquivalent(const SignalSchemaKey& other) const {
        return ClientName() == other.ClientName() && SerializedSchema() == other.SerializedSchema();
    }

    // Hashed value for this key.
    uint64_t Hash() const { return m_hash; }

    // We generally want the same key and serialized scheme to result in the same key id,
    //  but in the unlikely case that we ever get the same hashed value as another key
    //  with different client name and serialized schema, we can increment the salt value
    //  and compute a new key for a given registration.
    uint64_t GenerateReplacementHash();

 protected:
    uint64_t ComputeHash();

 private:
    const std::string m_client_name;
    const std::string m_serialized_schema;
    uint64_t m_salt = 0;
    uint64_t m_hash = 0;
};

}  // namespace data_buffer

}  // namespace client

}  // namespace bosdyn
