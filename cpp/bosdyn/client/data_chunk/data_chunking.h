/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include "bosdyn/client/service_client/result.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"

#include <bosdyn/api/data_chunk.pb.h>

namespace bosdyn {

namespace client {

/**
 * Create a std::string from a vector of data chunks.
 *
 * @param data_chunks Vector of data chunks to concatenate together.
 *
 * @return Result with a std::shared_ptr with a std::string.
 */
Result<std::string> StringFromDataChunks(
    const std::vector<const ::bosdyn::api::DataChunk*>& data_chunks);


/**
 * Create a message from deserializing concatenated data chunks.
 *
 * @param data_chunks Vector of chunks to concatenate and deserialize into a message.
 *
 * @return Result struct with std::shared_ptr with the deserialized message.
 */
template<class T>
Result<T> MessageFromDataChunks(
    const std::vector<const ::bosdyn::api::DataChunk*>& data_chunks){
    const Result<std::string>& result = StringFromDataChunks(data_chunks);
    if (!result) {
        return {result.status, {}};
    }

    T output;
    if (!output.ParseFromString(result.response)) {
        return {::bosdyn::common::Status(SDKErrorCode::GenericSDKError,
                       "Could not deserialize concatenated chunks into message"), {}};
    }
    return {::bosdyn::common::Status(SDKErrorCode::Success), std::move(output)};
}


/**
 * Create a vector of data chunks from a std::string.
 *
 * @param data Input buffer to chunk into data chunks.
 * @param chunks Output argument with the vector of data chunks.
 *
 * @return None, output from method is returned in chunks argument.
 */
void StringToDataChunks(const std::string& data,
                        std::vector<::bosdyn::api::DataChunk>* chunks);


/**
 * Create a vector of data chunks from a templatized message.
 *
 * NOTE: this is not using a Result return type for consistency with the StringToDataChunk method.
 *
 * @param message Message to be serialized and chunked into 4MB chunks.
 * @param chunks Vector of chunks created from the serialized message.
 *
 * @return ::bosdyn::common::Status with any errors found.
 */
template<class T>
::bosdyn::common::Status MessageToDataChunks(const T& message,
                           std::vector<::bosdyn::api::DataChunk>* chunks) {
    std::string buffer;
    if (!message.SerializeToString(&buffer)) {
        return ::bosdyn::common::Status(SDKErrorCode::GenericSDKError, "Could not serialize message");
    }
    StringToDataChunks(buffer, chunks);
    return ::bosdyn::common::Status(SDKErrorCode::Success);
}

}  // namespace client

}  // namespace bosdyn
