#ifndef SERVER_DISPATCHER_H_
#define SERVER_DISPATCHER_H_
#include <memory>
#include "server/handler_context.h"

namespace rest {

class Dispatcher 
 : public std::enable_shared_from_this<Dispatcher> {
 public:
  explicit Dispatcher(HandlerContext handler_context);
  ~Dispatcher();

  void Schedule(RESTServer::connection_ptr);

 private:
  HandlerContext handler_context_;
};

} // namespace rest
#endif // SERVER_DISPATCHER_H_
