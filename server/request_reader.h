#ifndef SERVER_REQUEST_READER_H_
#define SERVER_REQUEST_READER_H_
#include <memory>
#include "server/handler_context.h"

namespace rest {

class RequestReader 
 : public std::enable_shared_from_this<RequestReader> {
 public:
  typedef std::function<void(HandlerContext, RESTServer::connection_ptr)> CompletedCallback;

  virtual ~RequestReader();

  RequestReader(HandlerContext handler_context, CompletedCallback cb);
  void operator()(RESTServer::connection::input_range input,
                  std::error_code ec,
                  size_t bytes_transferred,
                  RESTServer::connection_ptr conn);
  
 private:
  static constexpr std::size_t MAX_INPUT_BODY_SIZE = 2 << 16;

  HandlerContext handler_context_;
  CompletedCallback completed_callback_;
  size_t content_length_;
};

} // namespace rest
#endif // SERVER_REQUEST_READER_H_
