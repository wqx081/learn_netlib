#include "server/dispatcher.h"
#include "server/handler_chain.h"
#include "server/response_writer.h"
#include <glog/logging.h>

namespace rest {

Dispatcher::Dispatcher(HandlerContext handler_context)
  : handler_context_(std::move(handler_context)) {}

Dispatcher::~Dispatcher() {}


void Dispatcher::Schedule(RESTServer::connection_ptr conn) {
  PreFilterChain::GetInstance()->Execute(&handler_context_,
                                         [this]() {
    LOG(INFO) << "OnPreFilterChainCompleted()";
  });
  RoutedHandlerChain::GetInstance()->RoutingAndExecute(handler_context_.request.destination,
                                                       &handler_context_,
                                                       [this]() {
    LOG(INFO) << "OnRoutedHandlerChainCompleted()";
  });
  PostFilterChain::GetInstance()->Execute(&handler_context_,
                                          [this]() {
    LOG(INFO) << "OnPostFilterChainCompleted()";
  });

  std::shared_ptr<ResponseWriter> response_writer(new ResponseWriter(handler_context_));
  (*response_writer)(conn);
}

} // namespace rest
