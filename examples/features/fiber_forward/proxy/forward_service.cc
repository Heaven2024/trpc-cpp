//
//
// Tencent is pleased to support the open source community by making tRPC available.
//
// Copyright (C) 2023 THL A29 Limited, a Tencent company.
// All rights reserved.
//
// If you have downloaded a copy of the tRPC source code from Tencent,
// please note that tRPC source code is licensed under the  Apache 2.0 License,
// A copy of the Apache 2.0 License is included in this file.
//
//

#include "examples/features/fiber_forward/proxy/forward_service.h"

#include <memory>
#include <string>
#include <utility>

#include "trpc/client/client_context.h"
#include "trpc/client/make_client_context.h"
#include "trpc/client/trpc_client.h"
#include "trpc/coroutine/fiber.h"
#include "trpc/coroutine/fiber_latch.h"
#include "trpc/log/trpc_log.h"

namespace examples::forward {

ForwardServiceImpl::ForwardServiceImpl() {
  greeter_proxy_ =
      ::trpc::GetTrpcClient()->GetProxy<::trpc::test::helloworld::GreeterServiceProxy>("trpc.test.helloworld.Greeter");
  second_greeter_proxy_ = 
      ::trpc::GetTrpcClient()->GetProxy<::trpc::test::helloworld::SecondGreeterServiceProxy>("trpc.test.helloworld.SecondGreeter");

}

::trpc::Status ForwardServiceImpl::Route(::trpc::ServerContextPtr context,
                                         const ::trpc::test::helloworld::HelloRequest* request,
                                         ::trpc::test::helloworld::HelloReply* reply) {
  TRPC_FMT_INFO("Forward request:{}, req id:{}", request->msg(), context->GetRequestId());

  auto client_context = ::trpc::MakeClientContext(context, greeter_proxy_);

  ::trpc::test::helloworld::HelloRequest route_request;
  route_request.set_msg(request->msg());

  ::trpc::test::helloworld::HelloReply route_reply;

  // block current fiber, not block current fiber worker thread
  ::trpc::Status status = greeter_proxy_->SayHello(client_context, route_request, &route_reply);

  TRPC_FMT_INFO("Forward status:{}, route_reply:{}", status.ToString(), route_reply.msg());

  reply->set_msg(route_reply.msg());

  return status;
}

::trpc::Status ForwardServiceImpl::ParallelRoute(::trpc::ServerContextPtr context,
                                                 const ::trpc::test::helloworld::HelloRequest* request,
                                                 ::trpc::test::helloworld::HelloReply* reply) {
  TRPC_FMT_INFO("Forward request:{}, req id:{}", request->msg(), context->GetRequestId());

  // send two requests in parallel to helloworldserver

  int exe_count = 2;
  ::trpc::FiberLatch l(exe_count);

  std::vector<::trpc::test::helloworld::HelloReply> vec_final_reply;
  vec_final_reply.resize(exe_count);

  // int i = 0;
  // while (i < exe_count) {
  //   bool ret = ::trpc::StartFiberDetached([this, &l, &context, &request, i, &vec_final_reply] {

  //     std::string msg = request->msg();
  //     msg += ", index";
  //     msg += std::to_string(i);

  //     trpc::test::helloworld::HelloRequest request;
  //     request.set_msg(msg);

  //     auto client_context = ::trpc::MakeClientContext(context, greeter_proxy_);
  //     ::trpc::Status status = greeter_proxy_->SayHello(client_context, request, &vec_final_reply[i]);

  //     TRPC_FMT_INFO("Forward i: {}, status:{}, route_reply:{}", i, status.ToString(), vec_final_reply[i].msg());

  //     // when reduced to 0, the FiberLatch `Wait` operation will be notified
  //     l.CountDown();
  //   });

  //   if (!ret) {
  //     std::string msg("failed, index:");
  //     msg += std::to_string(i);

  //     vec_final_reply[i].set_msg("failed.");

  //     l.CountDown();
  //   }
  
  //   i += 1;
  // }
    bool ret1 = ::trpc::StartFiberDetached([this, &l, &context, &request, &vec_final_reply] {
    auto client_context = ::trpc::MakeClientContext(context, greeter_proxy_);
    ::trpc::Status status = greeter_proxy_->SayHello(client_context, *request, &vec_final_reply[0]);
    TRPC_FMT_INFO("[Request ID:{}] [Fiber 1] Greeter status:{}, reply:{}", client_context->GetRequestId(),   status.ToString(), vec_final_reply[0].msg());
    l.CountDown();
    
  });

    bool ret2 = ::trpc::StartFiberDetached([this, &l, &context, &request, &vec_final_reply] {
    auto client_context = ::trpc::MakeClientContext(context, second_greeter_proxy_);
    ::trpc::Status status = second_greeter_proxy_->SayHelloAgain(client_context, *request, &vec_final_reply[1]);
    TRPC_FMT_INFO("[Request ID:{}] [Fiber 2] SecondGreeter status:{}, reply:{}", client_context->GetRequestId(),status.ToString(), vec_final_reply[1].msg());
    l.CountDown();
  });
    if (!ret1 || !ret2) {
    reply->set_msg("Failed to start fibers.");
    // return ::trpc::kFailureStatus;
  }
  // wait for two requests to return
  // block current fiber, not block current fiber worker thread
  l.Wait();

  reply->set_msg("parallel result: ");
  for (size_t i = 0; i < vec_final_reply.size(); i++) {
    reply->set_msg(reply->msg() + vec_final_reply[i].msg());
  }

  return ::trpc::kSuccStatus;
}

}  // namespace examples::forward
