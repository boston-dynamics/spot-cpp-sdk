/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "bosdyn/client/data_buffer/data_buffer_error_codes.h"

#include "bosdyn/client/error_codes/proto_enum_to_stderror_macro_source.h"

DEFINE_PROTO_ENUM_ERRORCODE_IMPL_API(RecordTextMessagesResponse_Error_Type, valcode == 0)
DEFINE_PROTO_ENUM_ERRORCODE_IMPL_API(RecordOperatorCommentsResponse_Error_Type, valcode == 0)
DEFINE_PROTO_ENUM_ERRORCODE_IMPL_API(RecordDataBlobsResponse_Error_Type, valcode == 0)
DEFINE_PROTO_ENUM_ERRORCODE_IMPL_API(RecordSignalTicksResponse_Error_Type, valcode == 0)
DEFINE_PROTO_ENUM_ERRORCODE_IMPL_API(RecordEventsResponse_Error_Type, valcode == 0)
