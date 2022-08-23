/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/data_chunk/data_chunking.h"

namespace bosdyn {

namespace client {

void StringToDataChunks(const std::string& data,
                        std::vector<::bosdyn::api::DataChunk>* chunks) {
    int chunk_size = 4 * 1024 * 1024;
    int start_index = 0;
    int left;
    const char* buffer = data.c_str();

    int total_size = data.length();
    while(true) {
        ::bosdyn::api::DataChunk chunk;
        left = total_size - start_index;
        if ( left < chunk_size) {
            chunk.set_data(buffer + start_index, left);
            chunk.set_total_size(total_size);
            chunks->emplace_back(std::move(chunk));
            break;
        }
        chunk.set_data(buffer + start_index, chunk_size);
        chunk.set_total_size(total_size);
        chunks->emplace_back(std::move(chunk));
        start_index += chunk_size;
    }
}

Result<std::string> StringFromDataChunks(
    const std::vector<const ::bosdyn::api::DataChunk*>& data_chunks) {
    if (data_chunks.empty()) {
        return {::bosdyn::common::Status(SDKErrorCode::Success), ""};
    }

    std::string full_data;
    int reported_total_size = data_chunks[0]->total_size();
    for (auto chunk : data_chunks) {
        if (chunk->total_size() != reported_total_size) {
            return {::bosdyn::common::Status(SDKErrorCode::GenericSDKError,
                           "Mismatch in reported total size in vector of data chunks"), ""};
        }
        full_data.append(chunk->data());
    }

    if (full_data.length() != reported_total_size) {
        return {::bosdyn::common::Status(SDKErrorCode::GenericSDKError, "Size mismatch in StringFromDataChunks"), ""};
    }

    return {::bosdyn::common::Status(SDKErrorCode::Success), std::move(full_data)};
}

}  // namespace client

}  // namespace bosdyn
