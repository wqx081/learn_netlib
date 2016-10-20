#ifndef SERVER_RESPONSE_WRITER_H_
#define SERVER_RESPONSE_WRITER_H_
#include <memory>
#include "server/handler_context.h"

namespace rest {

class ResponseWriter 
 : public std::enable_shared_from_this<ResponseWriter> {
 public:
  explicit ResponseWriter(HandlerContext& handler_context);
  virtual ~ResponseWriter();

  void operator()(RESTServer::connection_ptr conn);
  void SendHeaders(RESTServer::connection_ptr conn);
  void SendBody(size_t offset, size_t need_send_size, RESTServer::connection_ptr conn);

 private:
  HandlerContext& handler_context_;
  static const size_t kMaxPerSendSize = 4096; // 4KB

  void HandleChunk(size_t prev_offset, size_t prev_need_send_size, size_t sent_size,
                   RESTServer::connection_ptr conn, const std::error_code& ec);
};

} // namespace rest
#endif // SERVER_RESPONSE_WRITER_H_
