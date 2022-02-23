/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#include "util.h"

namespace bosdyn {

namespace client {
namespace fault {
namespace util {

::bosdyn::api::ServiceFault MakeServiceFault(const std::string& fault_name, const std::string& service_name,
                                   const std::string& error_message,
                                   ::bosdyn::api::ServiceFault::Severity severity,
                                   const std::vector<std::string>& attributes) {
    ::bosdyn::api::ServiceFault service_fault;
    service_fault.mutable_fault_id()->set_fault_name(fault_name);
    service_fault.mutable_fault_id()->set_service_name(service_name);
    service_fault.set_error_message(error_message);
    service_fault.set_severity(severity);
    for (const auto& attribute : attributes) {
        *service_fault.add_attributes() = attribute;
    }
    return service_fault;
}

// Create a service fault object for a payload fault.
::bosdyn::api::ServiceFault MakePayloadFault(const std::string& fault_name, const std::string& payload_guid,
                                   const std::string& error_message,
                                   ::bosdyn::api::ServiceFault::Severity severity,
                                   const std::vector<std::string>& attributes) {
    ::bosdyn::api::ServiceFault service_fault;
    service_fault.mutable_fault_id()->set_fault_name(fault_name);
    service_fault.mutable_fault_id()->set_payload_guid(payload_guid);
    service_fault.set_error_message(error_message);
    service_fault.set_severity(severity);
    for (const auto& attribute : attributes) {
        *service_fault.add_attributes() = attribute;
    }
    return service_fault;
}

}  // namespace util
}  // namespace fault
}  // namespace client

}  // namespace bosdyn
