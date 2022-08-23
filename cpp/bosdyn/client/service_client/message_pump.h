/**
 * Copyright (c) 2022 Boston Dynamics, Inc.  All rights reserved.
 *
 * Downloading, reproducing, distributing or otherwise using the SDK Software
 * is subject to the terms and conditions of the Boston Dynamics Software
 * Development Kit License (20191101-BDSDK-SL).
 */


#pragma once

#include <grpcpp/grpcpp.h>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <atomic>
#include <functional>
#include <future>
#include <mutex>
#include <set>
#include <thread>

#include <bosdyn/api/header.pb.h>

#include "bosdyn/client/service_client/result.h"
#include "bosdyn/common/time.h"
#include "bosdyn/common/assert_precondition.h"
#include "bosdyn/client/error_codes/rpc_error_code.h"

// MessagePump and related classes form the basis of the asynchronous gRPC API client
// implementation.
//
// The primary advantage over the blocking gRPC approach is that fewer threads may be used, and the
// application can generally control when RPCs are processed by throttling the MessagePump updates
// appropriately.
//
// The primary disadvantages over the blocking gRPC approach are that control flow tends to be less
// clear and the application must also manage the message pump.
//
// In either case, most of the Robot client API interfaces support both blocking and asynchronous
// RPCs.
//
// There are many ways to use the MessagePump class. The most common patterns are listed below:
//
// 1) Simple Case, where everything is handled by the ClientSdk:
// ClientSdk creates a std::shared_ptr<MessagePump>, calls AutoUpdate,
// and passes it to each Robot instances it creates.  The pump is stopped
// when the Robot is destroyed.
//
// 2) Application controlled (eg, single-threaded):
// Application creates std::shared_ptr<MessagePump> and passes it to Robot/ServiceClient through
// EnsureRobot/EnsureServiceClient.
// Application either calls AutoUpdate to create a shared thread for all robots,
// OR calls Update method in the same thread for single threaded behavior.
// The pump is stopped when any Robot/ServiceClient is destroyed.
//

namespace bosdyn {

namespace client {

enum CallStatus { NotStarted, Called, Cancelled, Completed };

template <typename CallbackFunctionType, typename PromiseResultType>
void CancelHelper(
    CallbackFunctionType& callback, std::promise<Result<PromiseResultType>> promise,
    CallStatus* call_status) {
    switch (*call_status) {
        case CallStatus::NotStarted:
            promise.set_value(
                {::bosdyn::common::Status(RPCErrorCode::ClientCancelledOperationError,
                        "Canceled RPC is not started yet"), {}});
            *call_status = CallStatus::Cancelled;
            callback = nullptr;
            return;
        case CallStatus::Called:
            promise.set_value({::bosdyn::common::Status(RPCErrorCode::ClientCancelledOperationError,
                                      "RPC is canceled"), {}});
            *call_status = CallStatus::Cancelled;
            callback = nullptr;
            return;
        case CallStatus::Cancelled:
            return;
        case CallStatus::Completed:
            // RPC cannot be cancelled after the callback is called.
            return;
    }
}

// MessagePumpCallBase is the abstract class for RPCs being managed by the MessagePump.
class MessagePumpCallBase {
 public:
    virtual ~MessagePumpCallBase() = default;

    /**
     * Method called by MessagePump for every event raised in the CompletionQueue.
     *
     * This instance of MessagePumpCallBase will be deleted after OnCompletionQueueEvent returns
     * true.
     *
     * @param success Success value from AsyncNext method called on the completion queue.
     *
     * @return True after the callback function is called, false otherwise.
     */
    virtual bool OnCompletionQueueEvent(bool success) = 0;

    // Cancel sets the value of the m_promise so the application is not	blocked forever if it
    // waiting on the promise. It populates Result::ResponseType with the default constructor. This
    // means that the ResponseType has to have a public default constructor, or add friends if the
    // default constructor is private. See ClassNoCtorWithMembers class in
    // test_set_promise_error_cases.cpp.
    virtual void Cancel() = 0;

    // Get the ClientContext for the RPC call. Clients can use this to override how a gRPC call will
    // be done - such as compression options, or specifying whether the request is idempotent. The
    // context should only be used and modified prior to Call being executed.
    grpc::ClientContext* context() {
        std::lock_guard<std::mutex> lock(m_call_mutex);
        BOSDYN_ASSERT_PRECONDITION(m_call_status == CallStatus::NotStarted, "Message pump cannot be started more than once.");
        return &m_context;
    }

 protected:
    friend class MessagePump;


    grpc::ClientContext m_context;
    grpc::Status m_status;
    std::mutex m_call_mutex;
    grpc::CompletionQueue* m_cq = nullptr;
    CallStatus m_call_status;

};

template <typename Request, typename Response, typename PromiseResultType>
class RequestStreamCall : public MessagePumpCallBase {
 public:
    typedef std::function<void(RequestStreamCall*, const std::vector<Request>&&,
                               Response&&, const grpc::Status&,
                               std::promise<Result<PromiseResultType>>)>
        RequestStreamCallbackFunction;

    typedef grpc::ClientAsyncWriter<Request> AsyncWriter;
    typedef std::function<std::unique_ptr<AsyncWriter>(
        grpc::ClientContext* context, Response* response, grpc::CompletionQueue* cq, void*)>
        RequestStreamRpcCallFunction;

    /**
     * Start the actual gRPC call. It should only be called once on a RequestStreamCall object.
     *
     * @param requests Vector of all requests to send to the server.
     * @param header   Request header. If streaming Data Chunks directly, this is the deserialized
     *                 request header.
     * @param rpc_call The RpcCallFunction object to start the RPC, and it will be invoked
     *                 immediately.
     * @param callback Callback function which will be invoked when the RPC completes on the same
     *                 thread as the MessagePump.
     * @param promise Promise to be set with the status and the response.
     * @param type_name Deserialized request type name.
     *
     * @return True if successful, false otherwise.
     */
    void Start(std::vector<Request>&& requests, const ::bosdyn::api::RequestHeader& header, const RequestStreamRpcCallFunction& rpc_call,
               const RequestStreamCallbackFunction& callback,
               std::promise<Result<PromiseResultType>> promise, const std::string& type_name) {
        std::lock_guard<std::mutex> lock(m_call_mutex);
        // This method should not be called with an empty list of requests.
        BOSDYN_ASSERT_PRECONDITION(!requests.empty(), "Request cannot be empty.");
        // Start should ONLY be called if the status not started.
        BOSDYN_ASSERT_PRECONDITION(m_call_status == CallStatus::NotStarted, "Message pump cannot be started multiple times.");
        m_requests = std::move(requests);
        m_callback = callback;
        m_promise = std::move(promise);
        m_request_writer = rpc_call(&m_context, &m_response, m_cq, this);
        m_next_step = NextStep::WriteRequest;
        m_call_status = CallStatus::Called;
    }

    void Cancel() override {
        std::lock_guard<std::mutex> lock(m_call_mutex);
        CancelHelper(m_callback, std::move(m_promise), &m_call_status);
    }

    ~RequestStreamCall() = default;

 private:
    friend class MessagePump;

    explicit RequestStreamCall(grpc::CompletionQueue* cq
    ) :
    m_next_request_to_write(0), m_next_step(NextStep::WriteRequest)
    {
        m_cq = cq;
        m_call_status = CallStatus::NotStarted;
        BOSDYN_ASSERT_PRECONDITION(m_cq != nullptr, "No completion queue.");
    }

    /**
     * In order to coordinate with the CompletionQueue, we need to keep track of what the
     * OnCompletionQueueEvent method should do when it is called by the CompletionQueue for each
     * event raised.
     * This enum defines all the possible steps for the OnCompletionQueueEvent method.
     */
    enum NextStep { WriteRequest, CallWritesDone, CallFinish, CallCallback };

    /**
     * Method called by MessagePump for every event raised in the CompletionQueue.
     *
     * This instance of MessagePumpCallBase will be deleted after OnCompletionQueueEvent returns
     * true. The order of operations for this method is:
     * 1) Write requests.
     * 2) Call WritesDone when there are no more requests to write.
     * 3) Call Finish.
     * 4) Call callback. Return true here, allowing the MessagePump to delete this instance.
     *
     * @param success Success value associated with the event from the CompletionQueue.
     *
     * @return True after callback is called, meaning everything related to the call is completed,
     *         false otherwise. A true return from this function will cause MessagePump to delete
     *         this instance.
     */
    bool OnCompletionQueueEvent(bool success) override {
        std::lock_guard<std::mutex> lock(m_call_mutex);

        if (m_call_status == CallStatus::Cancelled) {
            return true;
        }

        switch (m_next_step) {
            case NextStep::WriteRequest:
                if (success) {
                    m_request_writer->Write(m_requests[m_next_request_to_write], this);
                    m_next_request_to_write++;
                    if (m_next_request_to_write == m_requests.size()) {
                        m_next_step = NextStep::CallWritesDone;
                    }
                } else {
                    m_request_writer->Finish(&m_status, this);
                    m_next_step = NextStep::CallCallback;
                }
                return false;

            case NextStep::CallWritesDone:
                m_request_writer->WritesDone(this);
                m_next_step = NextStep::CallFinish;
                return false;

            case NextStep::CallFinish:
                m_request_writer->Finish(&m_status, this);
                m_next_step = NextStep::CallCallback;
                return false;

            case NextStep::CallCallback:
                if (m_callback != nullptr) {
                    m_callback(this, std::move(m_requests), std::move(m_response), m_status,
                               std::move(m_promise));
                    m_call_status = CallStatus::Completed;
                }
                return true;
        }

        // NOTE: This line is never reached.
        return false;
    }

    std::vector<Request> m_requests;
    std::unique_ptr<grpc::ClientAsyncWriter<Request>> m_request_writer;
    unsigned int m_next_request_to_write;
    Response m_response;
    NextStep m_next_step;
    RequestStreamCallbackFunction m_callback;
    std::promise<Result<PromiseResultType>> m_promise;
};

template <typename Request, typename Response, typename PromiseResultType>
class ResponseStreamCall : public MessagePumpCallBase {
 public:
    typedef std::function<void(ResponseStreamCall*, const Request&, std::vector<Response>&&,
                               const grpc::Status&, std::promise<Result<PromiseResultType>>)>
        ResponseStreamCallbackFunction;
    typedef grpc::ClientAsyncReader<Response> AsyncReader;
    typedef std::function<std::unique_ptr<AsyncReader>(grpc::ClientContext*, const Request&,
                                                       grpc::CompletionQueue*, void*)>
        ResponseStreamRpcCallFunction;

    /**
     * Start the actual gRPC call. It should only be called once on a ResponseStreamCall object.
     *
     * @param request Request to send to the server.
     * @param rpc_call The RpcCallFunction object to start the RPC, and it will be invoked
     *                 immediately.
     * @param callback Callback function which will be invoked when the RPC completes on the same
     *                 thread as the MessagePump.
     * @param promise Promise to be set with the status and the response.
     *
     * @return True if successful, false otherwise.
     */
    void Start(const Request& request, const ResponseStreamRpcCallFunction& rpc_call,
               const ResponseStreamCallbackFunction& callback,
               std::promise<Result<PromiseResultType>> promise) {
        std::lock_guard<std::mutex> lock(m_call_mutex);
        // Start should ONLY be called if the status not started.
        BOSDYN_ASSERT_PRECONDITION(m_call_status == CallStatus::NotStarted, "Message pump cannot be started multiple times.");
        m_request = request;
        m_callback = callback;
        m_promise = std::move(promise);
        m_response_reader = rpc_call(&m_context, m_request, m_cq, this);
        m_next_step = NextStep::StartRead;
        m_call_status = CallStatus::Called;
    }

    virtual void Cancel() override {
        std::lock_guard<std::mutex> lock(m_call_mutex);
        CancelHelper(m_callback, std::move(m_promise), &m_call_status);
    }

    ~ResponseStreamCall() = default;

 private:
    friend class MessagePump;

    explicit ResponseStreamCall(grpc::CompletionQueue* cq
    ) :
    m_next_step(NextStep::StartRead)
    {
        m_cq = cq;
        m_call_status = CallStatus::NotStarted;
        BOSDYN_ASSERT_PRECONDITION(m_cq != nullptr, "No completion queue.");
    }

    /**
     * In order to coordinate with the CompletionQueue, we need to keep track of what the
     * OnCompletionQueueEvent method should do when it is called by the CompletionQueue for each
     * event raise. This enum defines all the possible steps for the OnCompletionQueueEvent method.
     */
    enum NextStep { StartRead, ContinueReadOrCallFinish, CallCallback };

    /**
     * Method called by MessagePump for every event raised in the CompletionQueue.
     *
     * This instance of MessagePumpCallBase will be deleted after OnCompletionQueueEvent returns
     * true. The order of operations for this method is:
     * 1) Start reading responses
     * 2) Continue reading or call Finish when there are no responses
     * 3) Call callback. Return true here, allowing the MessagePump to delete this instance.
     *
     * @param success Success value associated with the event from the CompletionQueue.
     *
     * @return True after callback is called, meaning everything related to the call is completed,
     *         false otherwise. A true return from this function will cause MessagePump to delete
     *         this instance.
     */
    bool OnCompletionQueueEvent(bool success) override {
        std::lock_guard<std::mutex> lock(m_call_mutex);

        if (m_call_status == CallStatus::Cancelled) {
            return true;
        }

        switch (m_next_step) {
            case NextStep::StartRead:
                if (success) {
                    m_response_reader->Read(&m_last_response, this);
                    m_next_step = NextStep::ContinueReadOrCallFinish;
                } else {

                    // This happens when the request times out.
                    m_response_reader->Finish(&m_status, this);
                    m_next_step = NextStep::CallCallback;
                }
                return false;

            case NextStep::ContinueReadOrCallFinish:
                if (success) {
                    m_responses.push_back(std::move(m_last_response));
                    m_response_reader->Read(&m_last_response, this);
                } else {
                    // Done reading, call Finish.
                    m_response_reader->Finish(&m_status, this);
                    m_next_step = NextStep::CallCallback;
                }
                return false;

            case NextStep::CallCallback:
                // Everything is done, call the client callback with the responses.
                if (m_callback != nullptr) {
                    m_callback(this, m_request, std::move(m_responses), m_status,
                               std::move(m_promise));
                    m_call_status = CallStatus::Completed;
                }
                return true;
        }

        // NOTE: This line is never reached.
        return false;
    }

    Request m_request;
    std::unique_ptr<grpc::ClientAsyncReader<Response>> m_response_reader;
    // m_last_response is defined as a private var because it needs to persist over multiple
    // OnCompletionQueueEvent calls.
    Response m_last_response;
    std::vector<Response> m_responses;
    NextStep m_next_step;
    ResponseStreamCallbackFunction m_callback;
    std::promise<Result<PromiseResultType>> m_promise;
};

template <typename Request, typename Response, typename PromiseResultType>
class RequestResponseStreamCall : public MessagePumpCallBase {
 public:
    typedef std::function<void(RequestResponseStreamCall*, const std::vector<Request>&&,
                               std::vector<Response>&&, const grpc::Status&,
                               std::promise<Result<PromiseResultType>>)>
        RequestResponseStreamCallbackFunction;
    typedef grpc::ClientAsyncReaderWriter<Request, Response> AsyncReaderWriter;
    typedef std::function<std::unique_ptr<AsyncReaderWriter>(grpc::ClientContext*,
                                                             grpc::CompletionQueue*, void*)>
        RequestResponseStreamRpcCallFunction;

    /**
     * Start the actual gRPC call. It should only be called once on a RequestResponseStreamCall
     * object.
     *
     * @param requests Vector of requests to send to the server.
     * @param header   Request header. If streaming Data Chunks directly, this is the deserialized
     *                 request header.
     * @param rpc_call The RpcCallFunction object to start the RPC, and it will be invoked
     *                 immediately.
     * @param callback Callback function which will be invoked when the RPC completes on the same
     *                 thread as the MessagePump.
     * @param promise Promise to be set with the status and the response.
     * @param type_name Deserialized request type name.
     *
     * @return True if successful, false otherwise.
     */
    void Start(std::vector<Request>&& requests, const ::bosdyn::api::RequestHeader& header, const RequestResponseStreamRpcCallFunction& rpc_call,
               const RequestResponseStreamCallbackFunction& callback,
               std::promise<Result<PromiseResultType>> promise, const std::string& type_name) {
        std::lock_guard<std::mutex> lock(m_call_mutex);
        // This method should not be called with an empty list of requests.
        BOSDYN_ASSERT_PRECONDITION(!requests.empty(), "Request cannot be empty.");
        // Start should ONLY be called if the status not started.
        BOSDYN_ASSERT_PRECONDITION(
            m_call_status == CallStatus::NotStarted,
            "Message pump cannot be started multiple times.");
        m_requests = std::move(requests);
        m_callback = callback;
        m_promise = std::move(promise);
        m_reader_writer = rpc_call(&m_context, m_cq, this);
        m_next_step = NextStep::WriteRequest;
        m_call_status = CallStatus::Called;
        m_next_request_to_write = 0;
    }

    virtual void Cancel() override {
        std::lock_guard<std::mutex> lock(m_call_mutex);
        CancelHelper(m_callback, std::move(m_promise), &m_call_status);
    }

    ~RequestResponseStreamCall() = default;

 private:
    friend class MessagePump;

    explicit RequestResponseStreamCall(grpc::CompletionQueue* cq
    ) :
    m_next_request_to_write(0), m_next_step(NextStep::WriteRequest)
    {
        m_cq = cq;
        m_call_status = CallStatus::NotStarted;
        BOSDYN_ASSERT_PRECONDITION(m_cq != nullptr, "No completion queue.");
    }

    /**
     * In order to coordinate with the CompletionQueue, we need to keep track of what the
     * OnCompletionQueueEvent method should do when it is called by the CompletionQueue for each
     * event raise. This enum defines all the possible steps for the OnCompletionQueueEvent method.
     */
    enum NextStep {
        WriteRequest,
        CallWritesDone,
        StartRead,
        ContinueReadOrCallFinish,
        CallCallback
    };

    /**
     * Method called by MessagePump for every event raised in the CompletionQueue.
     *
     * This instance of MessagePumpCallBase will be deleted after OnCompletionQueueEvent returns
     * true. The order of operations for this method is:
     * 1) Write requests.
     * 2) Call WritesDone when there are no more requests to write.
     * 3) Start reading responses
     * 4) Continue reading or call Finish when there are no responses
     * 5) Call callback. Return true here, allowing the MessagePump to delete this instance.
     *
     * @param success Success value associated with the event from the CompletionQueue.
     *
     * @return True after callback is called, meaning everything related to the call is completed,
     *         false otherwise. A true return from this function will cause MessagePump to delete
     *         this instance.
     */
    bool OnCompletionQueueEvent(bool success) override {
        std::lock_guard<std::mutex> lock(m_call_mutex);

        if (m_call_status == CallStatus::Cancelled) {
            return true;
        }

        switch (m_next_step) {
            case NextStep::WriteRequest:
                if (success) {
                    m_reader_writer->Write(m_requests[m_next_request_to_write], this);
                    m_next_request_to_write++;
                    if (m_next_request_to_write == m_requests.size()) {
                        m_next_step = NextStep::CallWritesDone;
                    }
                } else {
                    m_reader_writer->Finish(&m_status, this);
                    m_next_step = NextStep::CallCallback;
                }

                return false;

            case NextStep::CallWritesDone:
                m_reader_writer->WritesDone(this);
                m_next_step = NextStep::StartRead;
                return false;

            case NextStep::StartRead:
                m_reader_writer->Read(&m_last_response, this);
                m_next_step = NextStep::ContinueReadOrCallFinish;
                return false;

            case NextStep::ContinueReadOrCallFinish:
                if (success) {
                    m_responses.push_back(std::move(m_last_response));
                    m_reader_writer->Read(&m_last_response, this);
                } else {
                    // Done reading, call Finish.
                    m_reader_writer->Finish(&m_status, this);
                    m_next_step = NextStep::CallCallback;
                    return false;
                }
                return false;

            case NextStep::CallCallback:
                // Everything is done, call the client callback with the responses.
                if (m_callback != nullptr) {
                    m_callback(this, std::move(m_requests), std::move(m_responses), m_status,
                               std::move(m_promise));
                    m_call_status = CallStatus::Completed;
                }
                return true;
        }

        // NOTE: This line is never reached.
        return false;
    }

    std::vector<Request> m_requests;
    std::unique_ptr<grpc::ClientAsyncReaderWriter<Request, Response>> m_reader_writer;
    unsigned int m_next_request_to_write;
    // m_last_response is defined as a private var because it needs to persist over multiple
    // OnCompletionQueueEvent calls.
    Response m_last_response;
    std::vector<Response> m_responses;
    NextStep m_next_step;
    RequestResponseStreamCallbackFunction m_callback;
    std::promise<Result<PromiseResultType>> m_promise;
};

/**
 * UnaryCall wraps a single request->response gRPC call.
 *
 * Clients instantiate UnaryCall objects using MessagePump::CreateUnaryCall(). The
 * MessagePump also destroys the UnaryCall object after the RPC completes.
 *
 * An RPC is initiated using UnaryCall::Call, with a passed in callback to be invoked when the
 * RPC completes. The callback will be invoked on the MessagePump thread.
 *
 * Clients can also "Cancel" a UnaryCall. Cancellation does not actually stop the RPC from
 * executing, but it does prevent the passed-in callback from being invoked. One case where this is
 * useful is when an object associated with the callback has died and would result in bad memory
 * de-reference.
 */
template <typename Request, typename Response, typename PromiseResultType>
class UnaryCall : public MessagePumpCallBase {
 public:
    typedef std::function<void(UnaryCall*, const Request&, Response&&, const grpc::Status&,
                               std::promise<Result<PromiseResultType>>)>
        CallbackFunction;
    typedef grpc::ClientAsyncResponseReader<Response> AsyncReader;
    typedef std::function<std::unique_ptr<AsyncReader>(grpc::ClientContext*, const Request&,
                                                       grpc::CompletionQueue*)>
        RpcCallFunction;

    /**
     * Start the actual gRPC call. It should only be called once on a UnaryCall object.
     *
     * @param request Request to send to the server.
     * @param rpc_call The RpcCallFunction object to start the RPC, and it will be invoked
     *                 immediately.
     * @param callback Callback function which will be invoked when the RPC completes on the same
     *                 thread as the MessagePump.
     * @param promise Promise to be set with the status and the response.
     *
     * @return True if successful, false otherwise.
     */
    void Start(const Request& request, const RpcCallFunction& rpc_call,
               const CallbackFunction& callback, std::promise<Result<PromiseResultType>> promise) {
        std::lock_guard<std::mutex> lock(m_call_mutex);
        // Start should ONLY be called if the status not started.
        BOSDYN_ASSERT_PRECONDITION(m_call_status == CallStatus::NotStarted, "Message pump cannot be started multiple times.");
        m_request = request;
        m_callback = callback;
        m_promise = std::move(promise);
        auto reader = rpc_call(&m_context, m_request, m_cq);
        m_call_status = CallStatus::Called;
        reader->Finish(&m_response, &m_status, this);
    }

    virtual void Cancel() override {
        std::lock_guard<std::mutex> lock(m_call_mutex);
        CancelHelper(m_callback, std::move(m_promise), &m_call_status);
    }

    ~UnaryCall() = default;
    // Disable default ctor and copy ctor.
    UnaryCall() = delete;
    UnaryCall(const UnaryCall&) = delete;

 private:
    friend class MessagePump;


    explicit UnaryCall(grpc::CompletionQueue* cq
    )
    {
        m_cq = cq;
        m_call_status = CallStatus::NotStarted;
        BOSDYN_ASSERT_PRECONDITION(m_cq != nullptr, "No completion queue.");
    }

    /**
     * Method called by MessagePump for every event raised in the CompletionQueue.
     *
     * This instance of MessagePumpCallBase will be deleted after OnCompletionQueueEvent returns
     * true. For UnaryCall instances, this method simply calls the callback function and returns
     * True.
     *
     * @param success Success value associated with the event from the CompletionQueue.
     *
     * @return Always True.
     */
    bool OnCompletionQueueEvent(bool success) override {
        std::lock_guard<std::mutex> lock(m_call_mutex);
        if (!success) {
        }

        if (m_call_status == CallStatus::Cancelled) {
            return true;
        }

        m_callback(this, m_request, std::move(m_response), m_status, std::move(m_promise));
        m_call_status = CallStatus::Completed;
        return true;
    }

    Request m_request;
    Response m_response;
    std::promise<Result<PromiseResultType>> m_promise;

    CallbackFunction m_callback;
};

/**
 * OutstandingCallTracker manages outstanding RPCs handled by the MessagePump.
 *
 * For example, a class which issues 1->N parallel RPCs could use OutstandingMessagePumpCalls to
 * keep track of which RPCs are active and how many are active. Also, when the lifetime of the
 * containing object ends, any outstanding RPCs will be canceled. This is done under the assumption
 * that callbacks will reference the containing class.
 */
class OutstandingCallTracker {
 public:
    OutstandingCallTracker() = default;
    ~OutstandingCallTracker() { CancelAll(); }

    // Cancel all tracked calls and remove them.
    void CancelAll();

    // Add a call to be tracked.
    void AddCall(std::unique_ptr<MessagePumpCallBase> call);

    // Remove a call from being tracked.
    void RemoveCall(MessagePumpCallBase* call);

    void RemoveAllCalls();

    // The number of calls being tracked.
    size_t Count() const;

 private:
    mutable std::mutex m_mutex;
    std::map<MessagePumpCallBase*, std::unique_ptr<MessagePumpCallBase>> m_calls;
};

enum UpdateStatus { Shutdown, Complete };

/**
 * MessagePump manages the gRPC completion queue.
 *
 * The gRPC completion queue can be thought of like a select statement loop. RPC calls are started
 * asynchronously, the completion queue is run, and RPC calls may complete via callbacks from the
 * completion queue. The primary benefit of this approach is that a number of outstanding RPCs can
 * all be managed with a single thread of execution.
 *
 * MessagePump expects that Update is only called from a single thread of execution. Creating and
 * calling RPCs can be done from any thread however.
 */
class MessagePump {
 public:
    MessagePump() = default;
    ~MessagePump() { RequestShutdown(); }


    // Process any completed RPCs in the completion queue for up to the duration milliseconds before
    // returning.  A typical application will have a thread which executes
    // MessagePump::CompleteOne in a loop.
    UpdateStatus CompleteOne(::bosdyn::common::Duration duration = std::chrono::milliseconds(100));

    // Start a thread and execute Update in a loop
    void AutoUpdate(::bosdyn::common::Duration duration);

    // Request that the pump shutdown. Once this happens, it can't be woken again.
    void RequestShutdown();

    // Creates and returns a OneTimeCall object. The returned OneTimeCall's object lifetime is
    // controlled by the MessagePump, and is guaranteed to be alive until after the Callback
    // function passed into Call completes execution.
    template <typename Request, typename Response, typename PromiseResultType>
    std::unique_ptr<UnaryCall<Request, Response, PromiseResultType>> CreateUnaryCall() {
        if (m_shutdown_requested) return nullptr;
        return std::unique_ptr<UnaryCall<Request, Response, PromiseResultType>>(
            new UnaryCall<Request, Response, PromiseResultType>(
                &m_completion_queue
            ));
    }

    template <typename Request, typename Response, typename Promise>
    std::unique_ptr<RequestStreamCall<Request, Response, Promise>> CreateRequestStreamCall() {
        if (m_shutdown_requested) return nullptr;
        return std::unique_ptr<RequestStreamCall<Request, Response, Promise>>(
            new RequestStreamCall<Request, Response, Promise>(
                &m_completion_queue
            ));
    }

    template <typename Request, typename Response, typename Promise>
    std::unique_ptr<ResponseStreamCall<Request, Response, Promise>> CreateResponseStreamCall() {
        if (m_shutdown_requested) return nullptr;
        return std::unique_ptr<ResponseStreamCall<Request, Response, Promise>>(
            new ResponseStreamCall<Request, Response, Promise>(
                &m_completion_queue
            ));
    }

    template <typename Request, typename Response, typename Promise>
    std::unique_ptr<RequestResponseStreamCall<Request, Response, Promise>> CreateRequestResponseStreamCall() {
        if (m_shutdown_requested) return nullptr;
        return std::unique_ptr<RequestResponseStreamCall<Request, Response, Promise>>(
            new RequestResponseStreamCall<Request, Response, Promise>(
                &m_completion_queue
            ));
    }

    // Add a call to be tracked.
    MessagePumpCallBase* AddCall(std::unique_ptr<MessagePumpCallBase> call) {
        MessagePumpCallBase* call_base_out = call.get();
        m_outstanding_calls.AddCall(std::move(call));
        return call_base_out;
    }

    size_t ActiveCalls() const { return m_outstanding_calls.Count(); }

 private:
    void UpdateLoop(::bosdyn::common::Duration duration);

    grpc::CompletionQueue m_completion_queue;
    bool m_has_auto_update_started = false;

    std::mutex m_shutdown_mutex;
    std::atomic<bool> m_shutdown_requested{false};

    std::unique_ptr<std::thread> m_auto_update_thread = nullptr;
    OutstandingCallTracker m_outstanding_calls;

};

}  // namespace client

}  // namespace bosdyn
