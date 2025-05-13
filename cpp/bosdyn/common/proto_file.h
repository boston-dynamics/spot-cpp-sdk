/**
 * Copyright (c) 2023 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <string>
#include <system_error>

#include <google/protobuf/message.h>

namespace bosdyn::common {

struct ParseOptions {
    // By default, linux uses "relatime" mounts, which only updates the access time occasionally to
    // try to reduce the amounts of writes when accessing files.  In some cases (caches) you do want
    // to ensure that the access time is updated.  This is done as a "best effort".  If the parsing
    // succeeds, but it was unable to update the access time, it will still return true.
    bool update_access_time = false;
    // An empty file will parse "successfully" for any proto without required fields.  Setting this
    // to true will ensure that at least some data was in the file, and it will return false if the
    // file is empty.
    bool ensure_non_empty = false;
};

struct WriteOptions {
    // Call fsync() successfully before closing the file.
    bool fsync_file = false;
};

// Parse a file into the provided message.
bool ParseMessageFromFile(const std::string& filename, google::protobuf::Message* message,
                          ParseOptions options = ParseOptions());

// Serialize a message to disk.
bool WriteMessageToFile(const std::string& filename, const google::protobuf::Message& message,
                        WriteOptions options = WriteOptions());

enum class ReadError {
    kSuccess = 0,
    kParseError,  // Message failed to parse.
    kEmptyFile,   // File was empty.
};

// Parse a file into the provided message.  Returns an error_code of what went wrong.
// The error code will either be a std::errc or a ReadError.
std::error_code ParseMessageFromFileWithError(const std::string& filename,
                                              google::protobuf::Message* message,
                                              ParseOptions options = ParseOptions());

enum class WriteError {
    kSuccess = 0,
    kSerializationError,  // Message failed to serialize.
};

// Serialize a message to disk.  Returns an error_code of what went wrong.
// The error code will either be a std::errc or a WriteError.
std::error_code WriteMessageToFileWithError(const std::string& filename,
                                            const google::protobuf::Message& message,
                                            WriteOptions options = WriteOptions());

std::error_code make_error_code(ReadError e);
std::error_code make_error_code(WriteError e);

}  // namespace bosdyn::common

namespace std {
template <>
struct is_error_code_enum<bosdyn::common::ReadError> : true_type {};
template <>
struct is_error_code_enum<bosdyn::common::WriteError> : true_type {};
}  // namespace std
