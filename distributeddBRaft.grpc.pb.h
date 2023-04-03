// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: distributeddBRaft.proto
// Original file comments:
// Copyright 2015 gRPC authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#ifndef GRPC_distributeddBRaft_2eproto__INCLUDED
#define GRPC_distributeddBRaft_2eproto__INCLUDED

#include "distributeddBRaft.pb.h"

#include <functional>
#include <grpcpp/generic/async_generic_service.h>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/client_context.h>
#include <grpcpp/completion_queue.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/proto_utils.h>
#include <grpcpp/impl/rpc_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/support/status.h>
#include <grpcpp/support/stub_options.h>
#include <grpcpp/support/sync_stream.h>

namespace distributeddBRaft {

class DistributeddBRaft final {
 public:
  static constexpr char const* service_full_name() {
    return "distributeddBRaft.DistributeddBRaft";
  }
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    virtual ::grpc::Status Get(::grpc::ClientContext* context, const ::distributeddBRaft::ReadReq& request, ::distributeddBRaft::Response* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::distributeddBRaft::Response>> AsyncGet(::grpc::ClientContext* context, const ::distributeddBRaft::ReadReq& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::distributeddBRaft::Response>>(AsyncGetRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::distributeddBRaft::Response>> PrepareAsyncGet(::grpc::ClientContext* context, const ::distributeddBRaft::ReadReq& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::distributeddBRaft::Response>>(PrepareAsyncGetRaw(context, request, cq));
    }
    virtual ::grpc::Status Put(::grpc::ClientContext* context, const ::distributeddBRaft::WriteReq& request, ::distributeddBRaft::Response* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::distributeddBRaft::Response>> AsyncPut(::grpc::ClientContext* context, const ::distributeddBRaft::WriteReq& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::distributeddBRaft::Response>>(AsyncPutRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::distributeddBRaft::Response>> PrepareAsyncPut(::grpc::ClientContext* context, const ::distributeddBRaft::WriteReq& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::distributeddBRaft::Response>>(PrepareAsyncPutRaw(context, request, cq));
    }
    class async_interface {
     public:
      virtual ~async_interface() {}
      virtual void Get(::grpc::ClientContext* context, const ::distributeddBRaft::ReadReq* request, ::distributeddBRaft::Response* response, std::function<void(::grpc::Status)>) = 0;
      virtual void Get(::grpc::ClientContext* context, const ::distributeddBRaft::ReadReq* request, ::distributeddBRaft::Response* response, ::grpc::ClientUnaryReactor* reactor) = 0;
      virtual void Put(::grpc::ClientContext* context, const ::distributeddBRaft::WriteReq* request, ::distributeddBRaft::Response* response, std::function<void(::grpc::Status)>) = 0;
      virtual void Put(::grpc::ClientContext* context, const ::distributeddBRaft::WriteReq* request, ::distributeddBRaft::Response* response, ::grpc::ClientUnaryReactor* reactor) = 0;
    };
    typedef class async_interface experimental_async_interface;
    virtual class async_interface* async() { return nullptr; }
    class async_interface* experimental_async() { return async(); }
   private:
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::distributeddBRaft::Response>* AsyncGetRaw(::grpc::ClientContext* context, const ::distributeddBRaft::ReadReq& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::distributeddBRaft::Response>* PrepareAsyncGetRaw(::grpc::ClientContext* context, const ::distributeddBRaft::ReadReq& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::distributeddBRaft::Response>* AsyncPutRaw(::grpc::ClientContext* context, const ::distributeddBRaft::WriteReq& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::distributeddBRaft::Response>* PrepareAsyncPutRaw(::grpc::ClientContext* context, const ::distributeddBRaft::WriteReq& request, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub final : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());
    ::grpc::Status Get(::grpc::ClientContext* context, const ::distributeddBRaft::ReadReq& request, ::distributeddBRaft::Response* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::distributeddBRaft::Response>> AsyncGet(::grpc::ClientContext* context, const ::distributeddBRaft::ReadReq& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::distributeddBRaft::Response>>(AsyncGetRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::distributeddBRaft::Response>> PrepareAsyncGet(::grpc::ClientContext* context, const ::distributeddBRaft::ReadReq& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::distributeddBRaft::Response>>(PrepareAsyncGetRaw(context, request, cq));
    }
    ::grpc::Status Put(::grpc::ClientContext* context, const ::distributeddBRaft::WriteReq& request, ::distributeddBRaft::Response* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::distributeddBRaft::Response>> AsyncPut(::grpc::ClientContext* context, const ::distributeddBRaft::WriteReq& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::distributeddBRaft::Response>>(AsyncPutRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::distributeddBRaft::Response>> PrepareAsyncPut(::grpc::ClientContext* context, const ::distributeddBRaft::WriteReq& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::distributeddBRaft::Response>>(PrepareAsyncPutRaw(context, request, cq));
    }
    class async final :
      public StubInterface::async_interface {
     public:
      void Get(::grpc::ClientContext* context, const ::distributeddBRaft::ReadReq* request, ::distributeddBRaft::Response* response, std::function<void(::grpc::Status)>) override;
      void Get(::grpc::ClientContext* context, const ::distributeddBRaft::ReadReq* request, ::distributeddBRaft::Response* response, ::grpc::ClientUnaryReactor* reactor) override;
      void Put(::grpc::ClientContext* context, const ::distributeddBRaft::WriteReq* request, ::distributeddBRaft::Response* response, std::function<void(::grpc::Status)>) override;
      void Put(::grpc::ClientContext* context, const ::distributeddBRaft::WriteReq* request, ::distributeddBRaft::Response* response, ::grpc::ClientUnaryReactor* reactor) override;
     private:
      friend class Stub;
      explicit async(Stub* stub): stub_(stub) { }
      Stub* stub() { return stub_; }
      Stub* stub_;
    };
    class async* async() override { return &async_stub_; }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    class async async_stub_{this};
    ::grpc::ClientAsyncResponseReader< ::distributeddBRaft::Response>* AsyncGetRaw(::grpc::ClientContext* context, const ::distributeddBRaft::ReadReq& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::distributeddBRaft::Response>* PrepareAsyncGetRaw(::grpc::ClientContext* context, const ::distributeddBRaft::ReadReq& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::distributeddBRaft::Response>* AsyncPutRaw(::grpc::ClientContext* context, const ::distributeddBRaft::WriteReq& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::distributeddBRaft::Response>* PrepareAsyncPutRaw(::grpc::ClientContext* context, const ::distributeddBRaft::WriteReq& request, ::grpc::CompletionQueue* cq) override;
    const ::grpc::internal::RpcMethod rpcmethod_Get_;
    const ::grpc::internal::RpcMethod rpcmethod_Put_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    virtual ::grpc::Status Get(::grpc::ServerContext* context, const ::distributeddBRaft::ReadReq* request, ::distributeddBRaft::Response* response);
    virtual ::grpc::Status Put(::grpc::ServerContext* context, const ::distributeddBRaft::WriteReq* request, ::distributeddBRaft::Response* response);
  };
  template <class BaseClass>
  class WithAsyncMethod_Get : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithAsyncMethod_Get() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_Get() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Get(::grpc::ServerContext* /*context*/, const ::distributeddBRaft::ReadReq* /*request*/, ::distributeddBRaft::Response* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestGet(::grpc::ServerContext* context, ::distributeddBRaft::ReadReq* request, ::grpc::ServerAsyncResponseWriter< ::distributeddBRaft::Response>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_Put : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithAsyncMethod_Put() {
      ::grpc::Service::MarkMethodAsync(1);
    }
    ~WithAsyncMethod_Put() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Put(::grpc::ServerContext* /*context*/, const ::distributeddBRaft::WriteReq* /*request*/, ::distributeddBRaft::Response* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestPut(::grpc::ServerContext* context, ::distributeddBRaft::WriteReq* request, ::grpc::ServerAsyncResponseWriter< ::distributeddBRaft::Response>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(1, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_Get<WithAsyncMethod_Put<Service > > AsyncService;
  template <class BaseClass>
  class WithCallbackMethod_Get : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithCallbackMethod_Get() {
      ::grpc::Service::MarkMethodCallback(0,
          new ::grpc::internal::CallbackUnaryHandler< ::distributeddBRaft::ReadReq, ::distributeddBRaft::Response>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::distributeddBRaft::ReadReq* request, ::distributeddBRaft::Response* response) { return this->Get(context, request, response); }));}
    void SetMessageAllocatorFor_Get(
        ::grpc::MessageAllocator< ::distributeddBRaft::ReadReq, ::distributeddBRaft::Response>* allocator) {
      ::grpc::internal::MethodHandler* const handler = ::grpc::Service::GetHandler(0);
      static_cast<::grpc::internal::CallbackUnaryHandler< ::distributeddBRaft::ReadReq, ::distributeddBRaft::Response>*>(handler)
              ->SetMessageAllocator(allocator);
    }
    ~WithCallbackMethod_Get() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Get(::grpc::ServerContext* /*context*/, const ::distributeddBRaft::ReadReq* /*request*/, ::distributeddBRaft::Response* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* Get(
      ::grpc::CallbackServerContext* /*context*/, const ::distributeddBRaft::ReadReq* /*request*/, ::distributeddBRaft::Response* /*response*/)  { return nullptr; }
  };
  template <class BaseClass>
  class WithCallbackMethod_Put : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithCallbackMethod_Put() {
      ::grpc::Service::MarkMethodCallback(1,
          new ::grpc::internal::CallbackUnaryHandler< ::distributeddBRaft::WriteReq, ::distributeddBRaft::Response>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::distributeddBRaft::WriteReq* request, ::distributeddBRaft::Response* response) { return this->Put(context, request, response); }));}
    void SetMessageAllocatorFor_Put(
        ::grpc::MessageAllocator< ::distributeddBRaft::WriteReq, ::distributeddBRaft::Response>* allocator) {
      ::grpc::internal::MethodHandler* const handler = ::grpc::Service::GetHandler(1);
      static_cast<::grpc::internal::CallbackUnaryHandler< ::distributeddBRaft::WriteReq, ::distributeddBRaft::Response>*>(handler)
              ->SetMessageAllocator(allocator);
    }
    ~WithCallbackMethod_Put() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Put(::grpc::ServerContext* /*context*/, const ::distributeddBRaft::WriteReq* /*request*/, ::distributeddBRaft::Response* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* Put(
      ::grpc::CallbackServerContext* /*context*/, const ::distributeddBRaft::WriteReq* /*request*/, ::distributeddBRaft::Response* /*response*/)  { return nullptr; }
  };
  typedef WithCallbackMethod_Get<WithCallbackMethod_Put<Service > > CallbackService;
  typedef CallbackService ExperimentalCallbackService;
  template <class BaseClass>
  class WithGenericMethod_Get : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithGenericMethod_Get() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_Get() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Get(::grpc::ServerContext* /*context*/, const ::distributeddBRaft::ReadReq* /*request*/, ::distributeddBRaft::Response* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_Put : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithGenericMethod_Put() {
      ::grpc::Service::MarkMethodGeneric(1);
    }
    ~WithGenericMethod_Put() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Put(::grpc::ServerContext* /*context*/, const ::distributeddBRaft::WriteReq* /*request*/, ::distributeddBRaft::Response* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithRawMethod_Get : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawMethod_Get() {
      ::grpc::Service::MarkMethodRaw(0);
    }
    ~WithRawMethod_Get() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Get(::grpc::ServerContext* /*context*/, const ::distributeddBRaft::ReadReq* /*request*/, ::distributeddBRaft::Response* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestGet(::grpc::ServerContext* context, ::grpc::ByteBuffer* request, ::grpc::ServerAsyncResponseWriter< ::grpc::ByteBuffer>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithRawMethod_Put : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawMethod_Put() {
      ::grpc::Service::MarkMethodRaw(1);
    }
    ~WithRawMethod_Put() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Put(::grpc::ServerContext* /*context*/, const ::distributeddBRaft::WriteReq* /*request*/, ::distributeddBRaft::Response* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestPut(::grpc::ServerContext* context, ::grpc::ByteBuffer* request, ::grpc::ServerAsyncResponseWriter< ::grpc::ByteBuffer>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(1, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithRawCallbackMethod_Get : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawCallbackMethod_Get() {
      ::grpc::Service::MarkMethodRawCallback(0,
          new ::grpc::internal::CallbackUnaryHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::grpc::ByteBuffer* request, ::grpc::ByteBuffer* response) { return this->Get(context, request, response); }));
    }
    ~WithRawCallbackMethod_Get() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Get(::grpc::ServerContext* /*context*/, const ::distributeddBRaft::ReadReq* /*request*/, ::distributeddBRaft::Response* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* Get(
      ::grpc::CallbackServerContext* /*context*/, const ::grpc::ByteBuffer* /*request*/, ::grpc::ByteBuffer* /*response*/)  { return nullptr; }
  };
  template <class BaseClass>
  class WithRawCallbackMethod_Put : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawCallbackMethod_Put() {
      ::grpc::Service::MarkMethodRawCallback(1,
          new ::grpc::internal::CallbackUnaryHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::grpc::ByteBuffer* request, ::grpc::ByteBuffer* response) { return this->Put(context, request, response); }));
    }
    ~WithRawCallbackMethod_Put() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Put(::grpc::ServerContext* /*context*/, const ::distributeddBRaft::WriteReq* /*request*/, ::distributeddBRaft::Response* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* Put(
      ::grpc::CallbackServerContext* /*context*/, const ::grpc::ByteBuffer* /*request*/, ::grpc::ByteBuffer* /*response*/)  { return nullptr; }
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_Get : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithStreamedUnaryMethod_Get() {
      ::grpc::Service::MarkMethodStreamed(0,
        new ::grpc::internal::StreamedUnaryHandler<
          ::distributeddBRaft::ReadReq, ::distributeddBRaft::Response>(
            [this](::grpc::ServerContext* context,
                   ::grpc::ServerUnaryStreamer<
                     ::distributeddBRaft::ReadReq, ::distributeddBRaft::Response>* streamer) {
                       return this->StreamedGet(context,
                         streamer);
                  }));
    }
    ~WithStreamedUnaryMethod_Get() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status Get(::grpc::ServerContext* /*context*/, const ::distributeddBRaft::ReadReq* /*request*/, ::distributeddBRaft::Response* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status StreamedGet(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::distributeddBRaft::ReadReq,::distributeddBRaft::Response>* server_unary_streamer) = 0;
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_Put : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithStreamedUnaryMethod_Put() {
      ::grpc::Service::MarkMethodStreamed(1,
        new ::grpc::internal::StreamedUnaryHandler<
          ::distributeddBRaft::WriteReq, ::distributeddBRaft::Response>(
            [this](::grpc::ServerContext* context,
                   ::grpc::ServerUnaryStreamer<
                     ::distributeddBRaft::WriteReq, ::distributeddBRaft::Response>* streamer) {
                       return this->StreamedPut(context,
                         streamer);
                  }));
    }
    ~WithStreamedUnaryMethod_Put() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status Put(::grpc::ServerContext* /*context*/, const ::distributeddBRaft::WriteReq* /*request*/, ::distributeddBRaft::Response* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status StreamedPut(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::distributeddBRaft::WriteReq,::distributeddBRaft::Response>* server_unary_streamer) = 0;
  };
  typedef WithStreamedUnaryMethod_Get<WithStreamedUnaryMethod_Put<Service > > StreamedUnaryService;
  typedef Service SplitStreamedService;
  typedef WithStreamedUnaryMethod_Get<WithStreamedUnaryMethod_Put<Service > > StreamedService;
};

}  // namespace distributeddBRaft


#endif  // GRPC_distributeddBRaft_2eproto__INCLUDED
