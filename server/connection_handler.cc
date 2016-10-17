#include "server/connection_handler.h"
#include <memory>
#include <boost/lexical_cast.hpp>
#include <glog/logging.h>

namespace rest {


AsyncConnectionHandler::AsyncConnectionHandler(const HandlerContext& handler_context,
                                               RESTServer::connection_ptr connection)
    : handler_context_(handler_context),
      connection_(connection),
      state_(START),
      request_body_length_(0) {
        for (const auto& header : handler_context_.request.headers) {
          if (boost::iequals(header.name, "content-length")) {
            request_body_length_ = boost::lexical_cast<int>(header.value);
            break;  
          }
        }
        //TODO(wqx): CHECK request body length
        request_body_left_length_ = request_body_length_;
      }

void AsyncConnectionHandler::ReadRequestBody() {
  DCHECK(state_ == START || state_ == READ_REQ_BODY);
  
  if (state_ == READ_REQ_BODY_COMPLETE) {
    // 调度 PreFilterChain
    // dispatcher->HandlePreFilterChain();
    return;
  }

  state_ = READ_REQ_BODY;
  auto self = this->shared_from_this();
  connection_->read([=] (RESTServer::connection::input_range range,
                         std::error_code ec,
                         size_t size,
                         RESTServer::connection_ptr conn) {
    self->DoReadRequestBody(range, ec, size, conn);
  });
}

void AsyncConnectionHandler::WriteResponseHeader() {
  connection_->set_status(RESTServer::connection::ok);
  connection_->set_headers(boost::make_iterator_range(handler_context_.response_headers.begin(), 
                                                      handler_context_.response_headers.end()));

}

void AsyncConnectionHandler::WriteResponseBody() {
  
}

void AsyncConnectionHandler::DoReadRequestBody(RESTServer::connection::input_range range,
                                               std::error_code ec,
                                               size_t size,
                                               RESTServer::connection_ptr conn) {
  (void) conn;
  if (!ec) {
    handler_context_.request_body.append(boost::begin(range), size);
    request_body_left_length_ = request_body_left_length_ - size;
    if (request_body_left_length_ > 0) {
      ReadRequestBody();
    } else {
      state_ = READ_REQ_BODY_COMPLETE;
      ReadRequestBody();
    }
  }
}



} // namespace rest
