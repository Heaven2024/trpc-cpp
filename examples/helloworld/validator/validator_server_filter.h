
#include "trpc/server/server_context.h"
#include "trpc/filter/filter.h"
#include "trpc/filter/filter_manager.h"

namespace trpc::overload_control {

class ValidatorServerFilter : public MessageServerFilter {
 public:
   std::string Name() override;

   std::vector<FilterPoint> GetFilterPoint() override;

   void operator()(FilterStatus& status, FilterPoint point, 
                   const ServerContextPtr& context) override;
 private:
  // Process requests by algorithm the result of which determine whether this request is allowed.
  void OnRequest(FilterStatus& status, const ServerContextPtr& context);
};

}

