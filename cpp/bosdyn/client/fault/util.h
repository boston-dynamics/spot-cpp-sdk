/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <bosdyn/api/service_fault.pb.h>

#include <string>
#include <vector>

namespace bosdyn {

namespace client {
namespace fault {
namespace util {

// Create a service fault object for a service level fault. This function will return a
// ServiceFault object with the service_fault field populated and the payload_guid field
// empty. This function should be used when the caller wants to specify a fault that is
// specific to a service.
::bosdyn::api::ServiceFault MakeServiceFault(const std::string& fault_name, const std::string& service_name,
                                   const std::string& error_message,
                                   ::bosdyn::api::ServiceFault::Severity severity,
                                   const std::vector<std::string>& attributes = {});

// Create a service fault object for a payload fault. This function will return a ServiceFault
// object with the payload_guid field populated and the service_name field empty. This
// function should be used when the caller wants to specify a fault that is specific to
// a payload.
::bosdyn::api::ServiceFault MakePayloadFault(const std::string& fault_name, const std::string& payload_guid,
                                   const std::string& error_message,
                                   ::bosdyn::api::ServiceFault::Severity severity,
                                   const std::vector<std::string>& attributes = {});

}  // namespace util
}  // namespace fault
}  // namespace client

}  // namespace bosdyn
