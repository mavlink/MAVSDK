// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: follow_me/follow_me.proto

#include "follow_me/follow_me.pb.h"
#include "follow_me/follow_me.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace mavsdk {
namespace rpc {
namespace follow_me {

static const char* FollowMeService_method_names[] = {
  "/mavsdk.rpc.follow_me.FollowMeService/GetConfig",
  "/mavsdk.rpc.follow_me.FollowMeService/SetConfig",
  "/mavsdk.rpc.follow_me.FollowMeService/IsActive",
  "/mavsdk.rpc.follow_me.FollowMeService/SetTargetLocation",
  "/mavsdk.rpc.follow_me.FollowMeService/GetLastLocation",
  "/mavsdk.rpc.follow_me.FollowMeService/Start",
  "/mavsdk.rpc.follow_me.FollowMeService/Stop",
};

std::unique_ptr< FollowMeService::Stub> FollowMeService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< FollowMeService::Stub> stub(new FollowMeService::Stub(channel, options));
  return stub;
}

FollowMeService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_GetConfig_(FollowMeService_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_SetConfig_(FollowMeService_method_names[1], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_IsActive_(FollowMeService_method_names[2], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_SetTargetLocation_(FollowMeService_method_names[3], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetLastLocation_(FollowMeService_method_names[4], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Start_(FollowMeService_method_names[5], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Stop_(FollowMeService_method_names[6], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status FollowMeService::Stub::GetConfig(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::GetConfigRequest& request, ::mavsdk::rpc::follow_me::GetConfigResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::mavsdk::rpc::follow_me::GetConfigRequest, ::mavsdk::rpc::follow_me::GetConfigResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_GetConfig_, context, request, response);
}

void FollowMeService::Stub::async::GetConfig(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::GetConfigRequest* request, ::mavsdk::rpc::follow_me::GetConfigResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::mavsdk::rpc::follow_me::GetConfigRequest, ::mavsdk::rpc::follow_me::GetConfigResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetConfig_, context, request, response, std::move(f));
}

void FollowMeService::Stub::async::GetConfig(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::GetConfigRequest* request, ::mavsdk::rpc::follow_me::GetConfigResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetConfig_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::mavsdk::rpc::follow_me::GetConfigResponse>* FollowMeService::Stub::PrepareAsyncGetConfigRaw(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::GetConfigRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::mavsdk::rpc::follow_me::GetConfigResponse, ::mavsdk::rpc::follow_me::GetConfigRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_GetConfig_, context, request);
}

::grpc::ClientAsyncResponseReader< ::mavsdk::rpc::follow_me::GetConfigResponse>* FollowMeService::Stub::AsyncGetConfigRaw(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::GetConfigRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncGetConfigRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status FollowMeService::Stub::SetConfig(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::SetConfigRequest& request, ::mavsdk::rpc::follow_me::SetConfigResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::mavsdk::rpc::follow_me::SetConfigRequest, ::mavsdk::rpc::follow_me::SetConfigResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_SetConfig_, context, request, response);
}

void FollowMeService::Stub::async::SetConfig(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::SetConfigRequest* request, ::mavsdk::rpc::follow_me::SetConfigResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::mavsdk::rpc::follow_me::SetConfigRequest, ::mavsdk::rpc::follow_me::SetConfigResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_SetConfig_, context, request, response, std::move(f));
}

void FollowMeService::Stub::async::SetConfig(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::SetConfigRequest* request, ::mavsdk::rpc::follow_me::SetConfigResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_SetConfig_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::mavsdk::rpc::follow_me::SetConfigResponse>* FollowMeService::Stub::PrepareAsyncSetConfigRaw(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::SetConfigRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::mavsdk::rpc::follow_me::SetConfigResponse, ::mavsdk::rpc::follow_me::SetConfigRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_SetConfig_, context, request);
}

::grpc::ClientAsyncResponseReader< ::mavsdk::rpc::follow_me::SetConfigResponse>* FollowMeService::Stub::AsyncSetConfigRaw(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::SetConfigRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncSetConfigRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status FollowMeService::Stub::IsActive(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::IsActiveRequest& request, ::mavsdk::rpc::follow_me::IsActiveResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::mavsdk::rpc::follow_me::IsActiveRequest, ::mavsdk::rpc::follow_me::IsActiveResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_IsActive_, context, request, response);
}

void FollowMeService::Stub::async::IsActive(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::IsActiveRequest* request, ::mavsdk::rpc::follow_me::IsActiveResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::mavsdk::rpc::follow_me::IsActiveRequest, ::mavsdk::rpc::follow_me::IsActiveResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_IsActive_, context, request, response, std::move(f));
}

void FollowMeService::Stub::async::IsActive(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::IsActiveRequest* request, ::mavsdk::rpc::follow_me::IsActiveResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_IsActive_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::mavsdk::rpc::follow_me::IsActiveResponse>* FollowMeService::Stub::PrepareAsyncIsActiveRaw(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::IsActiveRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::mavsdk::rpc::follow_me::IsActiveResponse, ::mavsdk::rpc::follow_me::IsActiveRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_IsActive_, context, request);
}

::grpc::ClientAsyncResponseReader< ::mavsdk::rpc::follow_me::IsActiveResponse>* FollowMeService::Stub::AsyncIsActiveRaw(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::IsActiveRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncIsActiveRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status FollowMeService::Stub::SetTargetLocation(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::SetTargetLocationRequest& request, ::mavsdk::rpc::follow_me::SetTargetLocationResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::mavsdk::rpc::follow_me::SetTargetLocationRequest, ::mavsdk::rpc::follow_me::SetTargetLocationResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_SetTargetLocation_, context, request, response);
}

void FollowMeService::Stub::async::SetTargetLocation(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::SetTargetLocationRequest* request, ::mavsdk::rpc::follow_me::SetTargetLocationResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::mavsdk::rpc::follow_me::SetTargetLocationRequest, ::mavsdk::rpc::follow_me::SetTargetLocationResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_SetTargetLocation_, context, request, response, std::move(f));
}

void FollowMeService::Stub::async::SetTargetLocation(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::SetTargetLocationRequest* request, ::mavsdk::rpc::follow_me::SetTargetLocationResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_SetTargetLocation_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::mavsdk::rpc::follow_me::SetTargetLocationResponse>* FollowMeService::Stub::PrepareAsyncSetTargetLocationRaw(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::SetTargetLocationRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::mavsdk::rpc::follow_me::SetTargetLocationResponse, ::mavsdk::rpc::follow_me::SetTargetLocationRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_SetTargetLocation_, context, request);
}

::grpc::ClientAsyncResponseReader< ::mavsdk::rpc::follow_me::SetTargetLocationResponse>* FollowMeService::Stub::AsyncSetTargetLocationRaw(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::SetTargetLocationRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncSetTargetLocationRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status FollowMeService::Stub::GetLastLocation(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::GetLastLocationRequest& request, ::mavsdk::rpc::follow_me::GetLastLocationResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::mavsdk::rpc::follow_me::GetLastLocationRequest, ::mavsdk::rpc::follow_me::GetLastLocationResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_GetLastLocation_, context, request, response);
}

void FollowMeService::Stub::async::GetLastLocation(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::GetLastLocationRequest* request, ::mavsdk::rpc::follow_me::GetLastLocationResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::mavsdk::rpc::follow_me::GetLastLocationRequest, ::mavsdk::rpc::follow_me::GetLastLocationResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetLastLocation_, context, request, response, std::move(f));
}

void FollowMeService::Stub::async::GetLastLocation(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::GetLastLocationRequest* request, ::mavsdk::rpc::follow_me::GetLastLocationResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetLastLocation_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::mavsdk::rpc::follow_me::GetLastLocationResponse>* FollowMeService::Stub::PrepareAsyncGetLastLocationRaw(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::GetLastLocationRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::mavsdk::rpc::follow_me::GetLastLocationResponse, ::mavsdk::rpc::follow_me::GetLastLocationRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_GetLastLocation_, context, request);
}

::grpc::ClientAsyncResponseReader< ::mavsdk::rpc::follow_me::GetLastLocationResponse>* FollowMeService::Stub::AsyncGetLastLocationRaw(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::GetLastLocationRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncGetLastLocationRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status FollowMeService::Stub::Start(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::StartRequest& request, ::mavsdk::rpc::follow_me::StartResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::mavsdk::rpc::follow_me::StartRequest, ::mavsdk::rpc::follow_me::StartResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_Start_, context, request, response);
}

void FollowMeService::Stub::async::Start(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::StartRequest* request, ::mavsdk::rpc::follow_me::StartResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::mavsdk::rpc::follow_me::StartRequest, ::mavsdk::rpc::follow_me::StartResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Start_, context, request, response, std::move(f));
}

void FollowMeService::Stub::async::Start(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::StartRequest* request, ::mavsdk::rpc::follow_me::StartResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Start_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::mavsdk::rpc::follow_me::StartResponse>* FollowMeService::Stub::PrepareAsyncStartRaw(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::StartRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::mavsdk::rpc::follow_me::StartResponse, ::mavsdk::rpc::follow_me::StartRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_Start_, context, request);
}

::grpc::ClientAsyncResponseReader< ::mavsdk::rpc::follow_me::StartResponse>* FollowMeService::Stub::AsyncStartRaw(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::StartRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncStartRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status FollowMeService::Stub::Stop(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::StopRequest& request, ::mavsdk::rpc::follow_me::StopResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::mavsdk::rpc::follow_me::StopRequest, ::mavsdk::rpc::follow_me::StopResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_Stop_, context, request, response);
}

void FollowMeService::Stub::async::Stop(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::StopRequest* request, ::mavsdk::rpc::follow_me::StopResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::mavsdk::rpc::follow_me::StopRequest, ::mavsdk::rpc::follow_me::StopResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Stop_, context, request, response, std::move(f));
}

void FollowMeService::Stub::async::Stop(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::StopRequest* request, ::mavsdk::rpc::follow_me::StopResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Stop_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::mavsdk::rpc::follow_me::StopResponse>* FollowMeService::Stub::PrepareAsyncStopRaw(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::StopRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::mavsdk::rpc::follow_me::StopResponse, ::mavsdk::rpc::follow_me::StopRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_Stop_, context, request);
}

::grpc::ClientAsyncResponseReader< ::mavsdk::rpc::follow_me::StopResponse>* FollowMeService::Stub::AsyncStopRaw(::grpc::ClientContext* context, const ::mavsdk::rpc::follow_me::StopRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncStopRaw(context, request, cq);
  result->StartCall();
  return result;
}

FollowMeService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      FollowMeService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< FollowMeService::Service, ::mavsdk::rpc::follow_me::GetConfigRequest, ::mavsdk::rpc::follow_me::GetConfigResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](FollowMeService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::mavsdk::rpc::follow_me::GetConfigRequest* req,
             ::mavsdk::rpc::follow_me::GetConfigResponse* resp) {
               return service->GetConfig(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      FollowMeService_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< FollowMeService::Service, ::mavsdk::rpc::follow_me::SetConfigRequest, ::mavsdk::rpc::follow_me::SetConfigResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](FollowMeService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::mavsdk::rpc::follow_me::SetConfigRequest* req,
             ::mavsdk::rpc::follow_me::SetConfigResponse* resp) {
               return service->SetConfig(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      FollowMeService_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< FollowMeService::Service, ::mavsdk::rpc::follow_me::IsActiveRequest, ::mavsdk::rpc::follow_me::IsActiveResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](FollowMeService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::mavsdk::rpc::follow_me::IsActiveRequest* req,
             ::mavsdk::rpc::follow_me::IsActiveResponse* resp) {
               return service->IsActive(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      FollowMeService_method_names[3],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< FollowMeService::Service, ::mavsdk::rpc::follow_me::SetTargetLocationRequest, ::mavsdk::rpc::follow_me::SetTargetLocationResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](FollowMeService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::mavsdk::rpc::follow_me::SetTargetLocationRequest* req,
             ::mavsdk::rpc::follow_me::SetTargetLocationResponse* resp) {
               return service->SetTargetLocation(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      FollowMeService_method_names[4],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< FollowMeService::Service, ::mavsdk::rpc::follow_me::GetLastLocationRequest, ::mavsdk::rpc::follow_me::GetLastLocationResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](FollowMeService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::mavsdk::rpc::follow_me::GetLastLocationRequest* req,
             ::mavsdk::rpc::follow_me::GetLastLocationResponse* resp) {
               return service->GetLastLocation(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      FollowMeService_method_names[5],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< FollowMeService::Service, ::mavsdk::rpc::follow_me::StartRequest, ::mavsdk::rpc::follow_me::StartResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](FollowMeService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::mavsdk::rpc::follow_me::StartRequest* req,
             ::mavsdk::rpc::follow_me::StartResponse* resp) {
               return service->Start(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      FollowMeService_method_names[6],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< FollowMeService::Service, ::mavsdk::rpc::follow_me::StopRequest, ::mavsdk::rpc::follow_me::StopResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](FollowMeService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::mavsdk::rpc::follow_me::StopRequest* req,
             ::mavsdk::rpc::follow_me::StopResponse* resp) {
               return service->Stop(ctx, req, resp);
             }, this)));
}

FollowMeService::Service::~Service() {
}

::grpc::Status FollowMeService::Service::GetConfig(::grpc::ServerContext* context, const ::mavsdk::rpc::follow_me::GetConfigRequest* request, ::mavsdk::rpc::follow_me::GetConfigResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status FollowMeService::Service::SetConfig(::grpc::ServerContext* context, const ::mavsdk::rpc::follow_me::SetConfigRequest* request, ::mavsdk::rpc::follow_me::SetConfigResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status FollowMeService::Service::IsActive(::grpc::ServerContext* context, const ::mavsdk::rpc::follow_me::IsActiveRequest* request, ::mavsdk::rpc::follow_me::IsActiveResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status FollowMeService::Service::SetTargetLocation(::grpc::ServerContext* context, const ::mavsdk::rpc::follow_me::SetTargetLocationRequest* request, ::mavsdk::rpc::follow_me::SetTargetLocationResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status FollowMeService::Service::GetLastLocation(::grpc::ServerContext* context, const ::mavsdk::rpc::follow_me::GetLastLocationRequest* request, ::mavsdk::rpc::follow_me::GetLastLocationResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status FollowMeService::Service::Start(::grpc::ServerContext* context, const ::mavsdk::rpc::follow_me::StartRequest* request, ::mavsdk::rpc::follow_me::StartResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status FollowMeService::Service::Stop(::grpc::ServerContext* context, const ::mavsdk::rpc::follow_me::StopRequest* request, ::mavsdk::rpc::follow_me::StopResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace mavsdk
}  // namespace rpc
}  // namespace follow_me
