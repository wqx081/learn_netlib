#include "server/response_writer.h"

#include "asio/error.hpp"
#include "asio/error_code.hpp"

namespace rest {

const size_t rest::ResponseWriter::kMaxPerSendSize;


ResponseWriter::ResponseWriter(HandlerContext& handler_context)
    : handler_context_(handler_context) {}

ResponseWriter::~ResponseWriter() {}

void ResponseWriter::operator()(RESTServer::connection_ptr conn) {
  if (handler_context_.response_body.size() > 0) {
    auto it = std::find_if(handler_context_.response_headers.begin(),
                           handler_context_.response_headers.end(),
                           [](const RESTServer::response_header& header) -> bool {
                             if (boost::iequals(header.name, "content-length")) {
                               return true;
                             }
                             return false;
    });
    if (it == handler_context_.response_headers.end()) {
      std::string content_len = std::to_string(handler_context_.response_body.size());
      handler_context_.response_headers.push_back({"Content-Length", content_len});
    }  
    SendHeaders(conn);
    SendBody(0, handler_context_.response_body.size(), conn);
  } else { // Empty Response
    ; //TODO(wqx): conn->write("");
  } 
}

void ResponseWriter::SendHeaders(RESTServer::connection_ptr conn) { 
  conn->set_status(RESTServer::connection::ok);
  conn->set_headers(boost::make_iterator_range(handler_context_.response_headers.begin(),
  handler_context_.response_headers.end()));
}

void ResponseWriter::SendBody(size_t offset, size_t need_send_size, RESTServer::connection_ptr conn) {
  size_t sent_size = std::min(need_send_size, kMaxPerSendSize);
  auto self = this->shared_from_this();
  conn->write(asio::const_buffers_1(
    handler_context_.response_body.data() + offset,
    sent_size),
    [=](const std::error_code& ec) {
      self->HandleChunk(offset, need_send_size, sent_size, conn, ec);
    });
}

void ResponseWriter::HandleChunk(size_t prev_offset, size_t prev_need_send_size, size_t sent_size,
                                 RESTServer::connection_ptr conn,
                                 const std::error_code& ec) {
  if (prev_need_send_size - sent_size <= 0) {
    return;
  }
  if (!ec) {
    SendBody(prev_offset + sent_size, prev_need_send_size - sent_size, conn);
  }
}


} // namespace rest
