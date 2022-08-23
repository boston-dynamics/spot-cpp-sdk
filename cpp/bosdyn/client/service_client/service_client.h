/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <grpcpp/grpcpp.h>

#include <future>
#include <memory>
#include <vector>

#include "bosdyn/client/service_client/message_pump.h"
#include "bosdyn/client/processors/request_processor_chain.h"
#include "bosdyn/client/processors/response_processor_chain.h"
#include "bosdyn/client/service_client/result.h"
#include "bosdyn/client/service_client/rpc_parameters.h"
#include "bosdyn/client/error_codes/rpc_error_code.h"
#include "bosdyn/client/error_codes/sdk_error_code.h"
#include "bosdyn/client/lease/lease_wallet.h"
#include "bosdyn/common/assert_precondition.h"
#include "bosdyn/client/lease/lease_processors.h"
#include "bosdyn/client/data_chunk/data_chunking.h"

namespace bosdyn {

namespace client {

// Base for any client version of a gRPC service running on robot.
class ServiceClient {
 public:
    ServiceClient() { m_RPC_parameters.timeout = kDefaultRPCTimeout; }

    virtual ~ServiceClient() = default;

    // QualityOfService, which can be used for network selection optimization.
    enum class QualityOfService {
        // The service is latency critical. For example, a service which controls the safety of the
        // robot would
        // be listed here.
        LATENCY_CRITICAL,

        // The service is normal level of service. Messages are not too large, and some latency is
        // tolerable.
        NORMAL,

        // The service uses large messages which do not need to be latency sensitive.
        BULK_THROUGHPUT,
    };
    virtual QualityOfService GetQualityOfService() const = 0;

    // Set the communication details for the service.
    virtual void SetComms(const std::shared_ptr<grpc::ChannelInterface>& channel) = 0;

    virtual void UpdateServiceFrom(RequestProcessorChain& request_processor_chain,
                                   ResponseProcessorChain& response_processor_chain,
                                   const std::shared_ptr<LeaseWallet>& /*lease_wallet*/) {
        m_request_processor_chain = request_processor_chain;
        m_response_processor_chain = response_processor_chain;
    }

    void AddCustomRequestProcessor(const std::shared_ptr<RequestProcessor>& request_processor) {
        m_request_processor_chain.AppendProcessor(request_processor);
    }
    void AddCustomResponseProcessor(const std::shared_ptr<ResponseProcessor>& response_processor) {
        m_response_processor_chain.AppendProcessor(response_processor);
    }

    // Set the message pump to be used.
    void SetMessagePump(std::shared_ptr<MessagePump> message_pump) {
        m_message_pump = message_pump;
    }

    void SetRPCParameters(const RPCParameters& parameters) {
        m_RPC_parameters = CombineRPCParameters(parameters);
    }

    void SetHeaderLoggingControl(LogRequestMode logging_control, ::bosdyn::api::RequestHeader* header) {
        header->set_disable_rpc_logging(logging_control == LogRequestMode::kDisabled);
    }
    template <class T>
    void SetLoggingControl(LogRequestMode logging_control, T* request) {
        if (logging_control != LogRequestMode::kDefault) {
            request->mutable_header()->set_disable_rpc_logging(logging_control ==
                                                               LogRequestMode::kDisabled);
        }
    }

 private:

    /**
     * Combine the passed RPCParameters with the ones stored in this class.
     *
     * @param parameters (bosdyn::client::RPCParameters): Parameters passed by the application with
     *     potential value updates.
     *
     * @return RPCParameters with the correct parameters to use for a specific RPC call.
     */
    RPCParameters CombineRPCParameters(const RPCParameters& parameters) const {
        RPCParameters parameters_to_use = m_RPC_parameters;
        if (parameters.logging_control != LogRequestMode::kDefault) {
            parameters_to_use.logging_control = parameters.logging_control;
        }
        if (parameters.timeout != kRPCTimeoutNotSpecified) {
            parameters_to_use.timeout = parameters.timeout;
        }
        return parameters_to_use;
    }

 protected:
    /**
     * Initiate the async UnaryCall and return the future to the promise.
     *
     * This method is called by all the async unary methods defined in the ServiceClient-derived
     * classes. It executes all the request processors on the request, and then it initializes the
     * RPC call through the MessagePump.
     *
     * @param request Request message to be sent through RPC.
     * @param rpc_call RPC function associated with this unary call.
     * @param callback Callback function defined in the client to call when the RPC completes.
     * @param result_promise Promise the callback function needs to set with the status and the
     *                       response.
     *
     * @returns Instance of UnaryCall created, or nullptr if errors occur.
     */
    template <typename Request, typename Response, typename PromiseResultType>
    MessagePumpCallBase* InitiateAsyncCall(
        Request& request,
        const typename ::bosdyn::client::UnaryCall<Request, Response, PromiseResultType>::RpcCallFunction&
            rpc_call,
        const typename ::bosdyn::client::UnaryCall<Request, Response, PromiseResultType>::CallbackFunction&
            callback,
        std::promise<Result<PromiseResultType>> result_promise,
        const RPCParameters& parameters) {
        BOSDYN_ASSERT_PRECONDITION(m_message_pump != nullptr,
                                   "Message pump cannot be null for request type %s",
                                   Request::GetDescriptor()->full_name().c_str());

        // The one_time pointer is deleted by MessagePump::Update after the callback function
        // returns
        auto one_time = m_message_pump->CreateUnaryCall<Request, Response, PromiseResultType>();
        if (!one_time) {
            result_promise.set_value(
                {::bosdyn::common::Status(RPCErrorCode::ClientCancelledOperationError, "MessagePump has shut down"),
                 {}});
            return nullptr;
        }

        RPCParameters parameters_to_use = CombineRPCParameters(parameters);
        SetLoggingControl(parameters_to_use.logging_control, &request);
        one_time->context()->set_deadline(
            std::chrono::system_clock::now() +
            CONVERT_DURATION_FOR_GRPC(parameters_to_use.timeout));

        auto status = m_request_processor_chain.Process(one_time->context(),
                                                        request.mutable_header(), &request);
        if (!status) {
            result_promise.set_value({std::move(status), {}});
            return nullptr;
        }
        auto ret = one_time.get();
        m_message_pump->AddCall(std::move(one_time));
        ret->Start(request, rpc_call, callback, std::move(result_promise));
        return ret;
    }


    /**
     * Initiate the async RequestStreamCall and return the future to the promise.
     *
     * This method should be used when the streamed request has an embedded DataChunk message.
     *
     * This method is called by all the async methods with streaming requests defined in the
     * ServiceClient-derived classes. It executes all the request processors on the requests, and
     * then it initializes the RPC call through the MessagePump.
     *
     * @param requests Vector with Request messages to be sent through RPC. This method moves the
     *                 content of the vector into the RequestStreamCall instance.
     * @param rpc_call RPC function associated with this streaming call.
     * @param callback Callback function defined in the client to call when the RPC completes.
     * @param result_promise Promise the callback function needs to set with the status and the
     *                       response.
     *
     * @returns Instance of RequestStreamCall created, or nullptr if errors occur.
     */
    template <typename Request, typename Response, typename PromiseResultType>
    MessagePumpCallBase* InitiateRequestStreamAsyncCall(
        std::vector<Request>&& requests,
        const typename ::bosdyn::client::RequestStreamCall<
            Request, Response, PromiseResultType>::RequestStreamRpcCallFunction& rpc_call,
        const typename ::bosdyn::client::RequestStreamCall<
            Request, Response, PromiseResultType>::RequestStreamCallbackFunction& callback,
        std::promise<Result<PromiseResultType>> promise,
        const RPCParameters& parameters) {
        BOSDYN_ASSERT_PRECONDITION(m_message_pump != nullptr, "Message pump cannot be null.");
        RPCParameters parameters_to_use = CombineRPCParameters(parameters);

        // The one_time pointer is deleted by MessagePump::Update after the callback function
        // returns.
        auto one_time = SetupRequestStreamCall<Request,Response,PromiseResultType>(parameters_to_use.timeout);
        if (!one_time) {
            promise.set_value(
                {::bosdyn::common::Status(RPCErrorCode::ClientCancelledOperationError, "MessagePump has shut down"),
                 {}});
            return nullptr;
        }

        // Set logging and process requests.
        for (Request& request : requests) {
            SetLoggingControl(parameters_to_use.logging_control, &request);
            auto status = m_request_processor_chain.Process(one_time->context(),
                                                            request.mutable_header(), &request);
            if (!status) {
                promise.set_value({std::move(status), {}});
                return nullptr;
            }
        }

        // Make a copy of header before requests is moved.
        const auto header = requests[0].header();

        // Initialize RPC call.
        auto ret = one_time.get();
        m_message_pump->AddCall(std::move(one_time));
        ret->Start(std::move(requests), header, rpc_call, callback, std::move(promise), Request::descriptor()->full_name());
        return ret;
    }

    /**
     * Initiate the async RequestStreamCall and return the future to the promise.
     *
     * This method should be used when directly streaming DataChunks.
     *
     * This method is called by all the async methods with streaming requests defined in the
     * ServiceClient-derived classes. It executes all the request processors on the requests, and
     * then it initializes the RPC call through the MessagePump.
     *
     * @param request Deserialized request. This method serializes the data into a vector of
     *                DataChunks, then moves the contents of the vector into the
     *                RequestStreamCall instance.
     * @param rpc_call RPC function associated with this streaming call.
     * @param callback Callback function defined in the client to call when the RPC completes.
     * @param result_promise Promise the callback function needs to set with the status and the
     *                       response.
     *
     * @returns Instance of RequestStreamCall created, or nullptr if errors occur.
     */
    template <typename Request, typename Response, typename PromiseResultType>
    MessagePumpCallBase* InitiateRequestStreamAsyncCallWithChunking(
        Request&& request,
        const typename ::bosdyn::client::RequestStreamCall<
            ::bosdyn::api::DataChunk, Response, PromiseResultType>::RequestStreamRpcCallFunction& rpc_call,
        const typename ::bosdyn::client::RequestStreamCall<
            ::bosdyn::api::DataChunk, Response, PromiseResultType>::RequestStreamCallbackFunction& callback,
        std::promise<Result<PromiseResultType>> promise,
        const RPCParameters& parameters) {
        BOSDYN_ASSERT_PRECONDITION(m_message_pump != nullptr, "Message pump cannot be null.");
        RPCParameters parameters_to_use = CombineRPCParameters(parameters);

        // The one_time pointer is deleted by MessagePump::Update after the callback function
        // returns.
        auto one_time = SetupRequestStreamCall<::bosdyn::api::DataChunk,Response,PromiseResultType>(parameters_to_use.timeout);
        if (!one_time) {
            promise.set_value(
                {::bosdyn::common::Status(RPCErrorCode::ClientCancelledOperationError, "MessagePump has shut down"),
                 {}});
            return nullptr;
        }

        // Set logging and process request.
        SetLoggingControl(parameters_to_use.logging_control, &request);
        auto request_status = m_request_processor_chain.Process(one_time->context(),
                                                        request.mutable_header(), &request);
        if (!request_status) {
            promise.set_value({std::move(request_status), {}});
            return nullptr;
        }

        // Convert request to data chunks.
        std::vector<::bosdyn::api::DataChunk> chunks;
        ::bosdyn::common::Status chunk_status = MessageToDataChunks<Request>(request, &chunks);
        if (!chunk_status) {
            promise.set_value({std::move(chunk_status), {}});
            return nullptr;
        }

        // Initialize RPC call.
        auto ret = one_time.get();
        m_message_pump->AddCall(std::move(one_time));
        ret->Start(std::move(chunks), request.header(), rpc_call, callback, std::move(promise), Request::descriptor()->full_name());
        return ret;
    }

    /**
     * Creates a RequestStreamCall pointer
     *
     * This method is a helper function when initiating a request stream async call. It creates
     * the RequestStreamCall pointer and sets the RPC timeout.
     *
     * @param result_promise Promise the callback function needs to set with the status and the
     *                       response.
     * @param timeout RPC timeout.
     *
     * @returns Pointer to RequestStreamCall created, or nullptr if errors occur.
     */
    template <typename Request, typename Response, typename PromiseResultType>
    std::unique_ptr<RequestStreamCall<Request, Response, PromiseResultType>> SetupRequestStreamCall(
        const ::bosdyn::common::Duration& timeout) {
        auto one_time =
            m_message_pump->CreateRequestStreamCall<Request, Response, PromiseResultType>();
        if (!one_time) {
            return nullptr;
        }

        one_time->context()->set_deadline(std::chrono::system_clock::now() +
                                          CONVERT_DURATION_FOR_GRPC(timeout));

        return one_time;
    }

    /**
     * Initiate the async ResponseStreamCall and return the future to the promise.
     *
     * This method is called by all the async methods with streaming responses defined in the
     * ServiceClient-derived classes. It executes all the request processors on the request, and
     * then it initializes the RPC call through the MessagePump.
     *
     * @param request Request messages to be sent through RPC.
     * @param rpc_call RPC function associated with this streaming call.
     * @param callback Callback function defined in the client to call when the RPC completes.
     * @param result_promise Promise the callback function needs to set with the status and the
     *                       vector of responses.
     *
     * @returns Instance of ResponseStreamCall created, or nullptr if errors occur.
     */
    template <typename Request, typename Response, typename PromiseResultType>
    MessagePumpCallBase* InitiateResponseStreamAsyncCall(
        Request& request,
        const typename ::bosdyn::client::ResponseStreamCall<
            Request, Response, PromiseResultType>::ResponseStreamRpcCallFunction& rpc_call,
        const typename ::bosdyn::client::ResponseStreamCall<
            Request, Response, PromiseResultType>::ResponseStreamCallbackFunction& callback,
        std::promise<Result<PromiseResultType>> promise,
        const RPCParameters& parameters) {
        BOSDYN_ASSERT_PRECONDITION(m_message_pump != nullptr, "Message pump cannot be null.");

        // The one_time pointer is deleted by MessagePump::Update after the callback function
        // returns.
        auto one_time =
            m_message_pump->CreateResponseStreamCall<Request, Response, PromiseResultType>();
        if (!one_time) {
            promise.set_value(
                {::bosdyn::common::Status(RPCErrorCode::ClientCancelledOperationError, "MessagePump has shut down"),
                 {}});
            return nullptr;
        }

        RPCParameters parameters_to_use = CombineRPCParameters(parameters);
        SetLoggingControl(parameters_to_use.logging_control, &request);
        one_time->context()->set_deadline(
            std::chrono::system_clock::now() +
            CONVERT_DURATION_FOR_GRPC(parameters_to_use.timeout));

        auto status = m_request_processor_chain.Process(one_time->context(),
                                                        request.mutable_header(), &request);
        if (!status) {
            promise.set_value({std::move(status), {}});
            return nullptr;
        }
        auto ret = one_time.get();
        m_message_pump->AddCall(std::move(one_time));
        ret->Start(request, rpc_call, callback, std::move(promise));
        return ret;
    }

    /**
     * Initiate the async RequestResponseStreamCall and return the future to the promise.
     *
     * This method is called by all the async methods with streaming requests and responses defined
     * in the ServiceClient-derived classes. It executes all the request processors on the requests,
     * and then it initializes the RPC call through the MessagePump.
     *
     * @param requests Vector with Request messages to be sent through RPC. This method moves the
     *                 content of the vector into the RequestStreamCall instance.
     * @param rpc_call RPC function associated with this streaming call.
     * @param callback Callback function defined in the client to call when the RPC completes.
     * @param result_promise Promise the callback function needs to set with the status and the
     *                       vector of response.
     *
     * @returns Instance of RequestResponseStreamCall created, or nullptr if errors occur.
     */
    template <typename Request, typename Response, typename PromiseResultType>
    MessagePumpCallBase* InitiateRequestResponseStreamAsyncCall(
        std::vector<Request>&& requests,
        const typename ::bosdyn::client::RequestResponseStreamCall<
            Request, Response, PromiseResultType>::RequestResponseStreamRpcCallFunction& rpc_call,
        const typename ::bosdyn::client::RequestResponseStreamCall<
            Request, Response, PromiseResultType>::RequestResponseStreamCallbackFunction& callback,
        std::promise<Result<PromiseResultType>> promise,
        const RPCParameters& parameters) {
        BOSDYN_ASSERT_PRECONDITION(m_message_pump != nullptr, "Message pump cannot be null.");
        RPCParameters parameters_to_use = CombineRPCParameters(parameters);

        // The one_time pointer is deleted by MessagePump::Update after the callback function
        // returns.
        auto one_time = SetupRequestResponseStreamCall<Request, Response, PromiseResultType>(
            parameters_to_use.timeout);
        if (!one_time) {
            promise.set_value({::bosdyn::common::Status(RPCErrorCode::ClientCancelledOperationError,
                                                        "MessagePump has shut down"),
                               {}});
            return nullptr;
        }

        for (Request& request : requests) {
            SetLoggingControl(parameters_to_use.logging_control, &request);
            auto status = m_request_processor_chain.Process(one_time->context(),
                                                            request.mutable_header(), &request);
            if (!status) {
                promise.set_value({std::move(status), {}});
                return nullptr;
            }
        }

        auto ret = one_time.get();
        m_message_pump->AddCall(std::move(one_time));
        ret->Start(std::move(requests), rpc_call, callback, std::move(promise));
        return ret;
    }

    /**
     * Initiate the async RequestResponseStreamCall and return the future to the promise.
     *
     * This method should be used with bidirectional streaming rpc that directly stream DataChunks.
     *
     * This method is called by all the async methods with streaming requests and responses defined
     * in the ServiceClient-derived classes. It executes all the request processors on the requests,
     * and then it initializes the RPC call through the MessagePump.
     *
     * @param request Deserialized request. This method serializes the data into a vector of
     *                DataChunks, then moves the contents of the vector into the
     *                RequestStreamCall instance.
     * @param rpc_call RPC function associated with this streaming call.
     * @param callback Callback function defined in the client to call when the RPC completes.
     * @param result_promise Promise the callback function needs to set with the status and the
     *                       response.
     *
     * @returns Instance of RequestResponseStreamCall created, or nullptr if errors occur.
     */
    template <typename Request, typename PromiseResultType>
    MessagePumpCallBase* InitiateRequestResponseStreamAsyncCallWithChunking(
        Request&& request,
        const typename ::bosdyn::client::RequestResponseStreamCall<
            ::bosdyn::api::DataChunk, ::bosdyn::api::DataChunk,
            PromiseResultType>::RequestResponseStreamRpcCallFunction& rpc_call,
        const typename ::bosdyn::client::RequestResponseStreamCall<
            ::bosdyn::api::DataChunk, ::bosdyn::api::DataChunk,
            PromiseResultType>::RequestResponseStreamCallbackFunction& callback,
        std::promise<Result<PromiseResultType>> promise, const RPCParameters& parameters) {
        BOSDYN_ASSERT_PRECONDITION(m_message_pump != nullptr, "Message pump cannot be null.");
        RPCParameters parameters_to_use = CombineRPCParameters(parameters);

        // The one_time pointer is deleted by MessagePump::Update after the callback function
        // returns.
        auto one_time =
            SetupRequestResponseStreamCall<::bosdyn::api::DataChunk, ::bosdyn::api::DataChunk,
                                           PromiseResultType>(parameters_to_use.timeout);
        if (!one_time) {
            promise.set_value({::bosdyn::common::Status(RPCErrorCode::ClientCancelledOperationError,
                                                        "MessagePump has shut down"),
                               {}});
            return nullptr;
        }

        SetLoggingControl(parameters_to_use.logging_control, &request);
        auto status = m_request_processor_chain.Process(one_time->context(),
                                                        request.mutable_header(), &request);
        if (!status) {
            promise.set_value({std::move(status), {}});
            return nullptr;
        }

        // Convert request to data chunks.
        std::vector<::bosdyn::api::DataChunk> chunks;
        ::bosdyn::common::Status chunk_status = MessageToDataChunks<Request>(request, &chunks);
        if (!chunk_status) {
            promise.set_value({std::move(chunk_status), {}});
            return nullptr;
        }

        auto ret = one_time.get();
        m_message_pump->AddCall(std::move(one_time));
        ret->Start(std::move(chunks), request.header(), rpc_call, callback, std::move(promise),
                   Request::descriptor()->full_name());
        return ret;
    }

    /**
     * Creates a RequestResponseStreamCall pointer
     *
     * This method is a helper function when initiating a request response stream async call. It creates
     * the RequestResponseStreamCall pointer and sets the RPC timeout.
     *
     * @param result_promise Promise the callback function needs to set with the status and the
     *                       response.
     * @param timeout RPC timeout.
     *
     * @returns Pointer to RequestResponseStreamCall created, or nullptr if errors occur.
     */
    template <typename Request, typename Response, typename PromiseResultType>
    std::unique_ptr<RequestResponseStreamCall<Request, Response, PromiseResultType>>
    SetupRequestResponseStreamCall(const ::bosdyn::common::Duration& timeout) {
        auto one_time =
            m_message_pump->CreateRequestResponseStreamCall<Request, Response, PromiseResultType>();
        if (!one_time) {
            return nullptr;
        }

        one_time->context()->set_deadline(std::chrono::system_clock::now() +
                                          CONVERT_DURATION_FOR_GRPC(timeout));

        return one_time;
    }

    template <typename Response>
    ::bosdyn::common::Status RunOnlyResponseProcessors(const grpc::Status& status, const Response& response,
                             const std::error_code& response_status) {
        ::bosdyn::common::Status ret_status = ConvertGRPCStatus(status);
        if (!ret_status) {
            return ret_status;
        }

        return m_response_processor_chain.Process(status, response.header(), response);
    }

    template <typename Response>
    ::bosdyn::common::Status ProcessResponseWithLeaseAndGetFinalStatus(const grpc::Status& status,
                                                     const Response& response,
                                                     const std::error_code& response_status,
                                                     LeaseWallet* lease_wallet) {
        ::bosdyn::common::Status ret_status = RunOnlyResponseProcessors<Response>(status, response, response_status);
        // In the case where the processors return a bad status, just return that in the promise.
        if (!ret_status) {
            return ret_status;
        }

        // Next, run the lease use results checks.
        ret_status = ::bosdyn::client::ProcessResponseWithLeaseUseResult(response, lease_wallet);
        if (!ret_status) {
            // If the lease use result check returns a bad status, then exit with that status.
            return ret_status;
        }

        // Lastly, return the status from the response.
        ret_status = ::bosdyn::common::Status(response_status);
        return ret_status;
    }

    template <typename Response>
    ::bosdyn::common::Status ProcessResponseWithMultiLeaseAndGetFinalStatus(const grpc::Status& status,
                                                          const Response& response,
                                                          const std::error_code& response_status,
                                                          LeaseWallet* lease_wallet) {
        ::bosdyn::common::Status ret_status = RunOnlyResponseProcessors<Response>(status, response, response_status);
        // In the case where the processors return a bad status, just return that in the promise.
        if (!ret_status) {
            return ret_status;
        }

        // Next, run the lease use results checks for protos with "repeated LeaseUseResult results"
        // fields.
        ret_status = ::bosdyn::client::ProcessResponseForMultipleLease(response, lease_wallet);
        if (!ret_status) {
            // If the lease use result check returns a bad status, then exit with that status.
            return ret_status;
        }

        // Lastly, return the status from the response.
        ret_status = ::bosdyn::common::Status(response_status);
        return ret_status;
    }

    template <typename Response>
    ::bosdyn::common::Status ProcessResponseAndGetFinalStatus(const grpc::Status& status, const Response& response,
                                            const std::error_code& response_status) {
        ::bosdyn::common::Status ret_status = ConvertGRPCStatus(status);
        if (!ret_status) {
            return ret_status;
        }

        ret_status = m_response_processor_chain.Process(status, response.header(), response);
        if (!ret_status) {
            return ret_status;
        }

        // In this case, GRPC did not detect any issues, so we return the response status
        ret_status = ::bosdyn::common::Status(response_status);
        return ret_status;
    }

    template<typename Response>
    ::bosdyn::common::Status ProcessResponseVectorAndGetFinalStatus(const grpc::Status& status,
                                                  const std::vector<Response>& responses) {
        ::bosdyn::common::Status ret_status = ConvertGRPCStatus(status);
        if (!ret_status) {
            return ret_status;
        }

        for (const Response& response : responses) {
            ret_status = m_response_processor_chain.Process(status, response.header(), response);
            if (!ret_status) {
                return ret_status;
            }

            ret_status = ::bosdyn::common::Status(response.status());
            if (!ret_status) {
                return ret_status;
            }
        }

        // In this case, GRPC did not detect any issues, so we return the current success value of
        // the status.
        return ret_status;
    }

    // Mutex for managing protected/private members.
    std::mutex m_mutex;

    std::shared_ptr<MessagePump> m_message_pump = nullptr;

    // Client-level RPC parameters, these will be ignored if the RPC method passes valid parameters.
    RPCParameters m_RPC_parameters;

    // Comms stuff.
    RequestProcessorChain m_request_processor_chain;
    ResponseProcessorChain m_response_processor_chain;
};

}  // namespace client

}  // namespace bosdyn
