#include "server/request_reader.h"
#include <glog/logging.h>

namespace rest {

RequestReader::RequestReader(HandlerContext handler_context, CompletedCallback cb)
    : handler_context_(std::move(handler_context)),
      completed_callback_(cb),
      content_length_(0) {
  for (const auto& header : handler_context_.request.headers) {
    if (boost::iequals(header.name, "content-length")) {
      content_length_ = std::stoul(header.value);
      LOG(INFO) << "Content length: " << content_length_;
      break;
    }
  }
}

RequestReader::~RequestReader() {}

void RequestReader::operator() (RESTServer::connection::input_range input,
                                std::error_code ec,
                                size_t bytes_transferred,
                                RESTServer::connection_ptr conn) {
  LOG(INFO) << "Read Callback: " << bytes_transferred << "; ec = " << ec;
  if (ec == asio::error::eof) {
    return;
  }

  if (!ec) {

    if (empty(input)) {
       completed_callback_(std::move(handler_context_), conn);
        return;
    }

    handler_context_.request_body.append(boost::begin(input), bytes_transferred);

    if (handler_context_.request_body.size() > MAX_INPUT_BODY_SIZE) {

      conn->set_status(RESTServer::connection::bad_request);
      static RESTServer::response_header error_headers[] = {
        {"Connection", "close"}
      };
      conn->set_headers(
          boost::make_iterator_range(error_headers, error_headers + 1));
        conn->write("Bad Body");
      return;
    }

    LOG(INFO) << "Body: " << handler_context_.request_body << " body_size: "
                << handler_context_.request_body.size();
    if (handler_context_.request_body.size() == content_length_) {
      completed_callback_(std::move(handler_context_), conn);
      return;
    }

    LOG(INFO) << "Scheduling another read...";
    auto self = this->shared_from_this();
    conn->read([self](RESTServer::connection::input_range input,
                      std::error_code ec,
                      std::size_t bytes_transferred,
                      RESTServer::connection_ptr connection) {
      (*self)(input, ec, bytes_transferred, connection);
    });
  }
}

} // namespace rest
