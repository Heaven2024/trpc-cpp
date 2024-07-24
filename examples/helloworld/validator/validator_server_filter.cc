

#include "validator_server_filter.h"

namespace trpc::overload_control{
std::string ValidatorServerFilter::Name() {
  return "Validator_server";
}

std::vector<FilterPoint> ValidatorServerFilter::GetFilterPoint() {
  return {FilterPoint::SERVER_PRE_RPC_INVOKE, FilterPoint::SERVER_POST_RPC_INVOKE};
}

// void ValidatorServerFilter::operator()(FilterStatus& status, FilterPoint point, 
//                                     const ServerContextPtr& context) {
//   if (point == FilterPoint::SERVER_PRE_RPC_INVOKE) {
//     auto headers = context->GetPbReqTransInfo();
//     if (headers.find("X-Special-Header") == headers.end()) {
//       context->SetStatus(Status(TrpcRetCode::TRPC_SERVER_VALIDATE_ERR, 0, "rejected by server Validator"));
//       TRPC_FMT_ERROR_EVERY_SECOND("rejected by Validator");
//       status = FilterStatus::REJECT;
//       return;
//     }
//   }
//   // 放行请求
//   status = FilterStatus::CONTINUE;
// }
void ValidatorServerFilter::operator()(FilterStatus& status, FilterPoint point, const ServerContextPtr& context) {
  switch (point) {
    case FilterPoint::SERVER_PRE_RPC_INVOKE: {
      OnRequest(status, context);
      break;
    }
    default: {
      break;
    }
  }
}   

void ValidatorServerFilter::OnRequest(FilterStatus& status, const ServerContextPtr& context) {
  // if (TRPC_UNLIKELY(!context->GetStatus().OK())) {
  //   // If it is already a dirty request, it will not be processed further to ensure that the first error code is
  //   // not overwritten.
  //   return;
  // }
  // Get the current unfinished requests.
  // Compare the current unfinished requests with the maximum concurrency to determine whether interception is required.
    auto headers = context->GetPbReqTransInfo();
    if (headers.find("X-Special-Header") == headers.end()) {
      context->SetStatus(Status(TrpcRetCode::TRPC_SERVER_VALIDATE_ERR, 0, "rejected by server Validator"));
      TRPC_FMT_ERROR_EVERY_SECOND("rejected by Validator");
      status = FilterStatus::REJECT;
      return;
    }
  
  // 放行请求
  status = FilterStatus::CONTINUE;
  }


}