#ifndef SERVER_CONNECTION_HANDLER_H_
#define SERVER_CONNECTION_HANDLER_H_

#include <memory>
#include <string>

#include "server/handler_context.h"

namespace rest {

class ConnectionHandler {
 public:
  virtual ~ConnectionHandler() {}

  virtual void ReadRequestBody() = 0;
  virtual void WriteResponseHeader() = 0;
  virtual void WriteResponseBody() = 0;
};

class AsyncConnectionHandler 
  : public std::enable_shared_from_this<AsyncConnectionHandler>,
    public ConnectionHandler {

 public:
  enum State { 
    START = 0,
    READ_REQ_BODY,
    READ_REQ_BODY_COMPLETE,
    WRITE_RES_BODY,
    WRITE_RES_BODY_COMPLETE,
    // Errors      
    EXCEED_MAX_REQ_BODY_LENGTH,         
  };
  
  AsyncConnectionHandler(const HandlerContext& handler_context,
                         RESTServer::connection_ptr connection);
 
  virtual ~AsyncConnectionHandler() override {}

  virtual void ReadRequestBody() override; 
  virtual void WriteResponseHeader() override;
  virtual void WriteResponseBody() override;

  State state() const { return state_; }
  std::string StateToString() const;

 private:

  HandlerContext handler_context_;
  RESTServer::connection_ptr connection_;
  State state_;
  int request_body_length_;
  int request_body_left_length_;
  static const int kMaxRequestBodyLength = 1024 * 2;

  void DoReadRequestBody(RESTServer::connection::input_range range,
                         std::error_code ec,
                         size_t size,
                         RESTServer::connection_ptr);
};

} // namespace rest
#endif // SERVER_CONNECTION_HANDLER_H_
