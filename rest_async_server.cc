// Author: Wangqixiang (wangqx at mpreader dot com)
// Date: ...
//
#include <memory>
#include <boost/network/protocol/http/server.hpp>
#include <boost/lexical_cast.hpp>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cassert>

#include <glog/logging.h>

namespace http = boost::network::http;

struct RestAsyncHandler;
typedef http::server<RestAsyncHandler> RestAsyncServer;

// 主要用来做异步处理
struct ConnectionHandler : std::enable_shared_from_this<ConnectionHandler> {
  const RestAsyncServer::request& request_;
  std::string body_;
  static const int kMaxRequestBodyLength = 1024 * 2;

  // APIs
  ConnectionHandler(const RestAsyncServer::request& request) : request_(request), body_("") {}

  void operator() (RestAsyncServer::connection_ptr connection) {
    int content_length = 0;
    for (const auto& header : request_.headers) {
      if (boost::iequals(header.name, "content-length")) {
        content_length = boost::lexical_cast<int>(header.value);
        LOG(INFO) << "content_length: " << content_length;
        break;
      }
    }
    if (content_length >= kMaxRequestBodyLength) {
      connection->write("Hello, World");
      return;
    }

    // 0) Read Request
    // 1) Parse Request
    // 2) Do Something
    // 3) Write Response
    //
    ReadChunk(content_length, connection);
    // send headers
    static RestAsyncServer::response_header headers[] = {
      {"Content-Type", "text/plain"},
      {"Content-Length", "12"}
    };
    connection->set_status(RestAsyncServer::connection::ok);
    connection->set_headers(boost::make_iterator_range(headers, headers + 2));
    // send body
    connection->write("Hello, World");
  }

  void ReadChunk(size_t need_to_read, RestAsyncServer::connection_ptr connection) {
    auto self = this->shared_from_this();
    connection->read([=] (RestAsyncServer::connection::input_range range,
                          std::error_code ec, 
                          std::size_t size,
                          RestAsyncServer::connection_ptr conn) {
      self->HandlePostRead(range, ec, size, conn, need_to_read);
    });
  }

  void HandlePostRead(RestAsyncServer::connection::input_range range,
                      std::error_code ec,
                      std::size_t size,
                      RestAsyncServer::connection_ptr conn,
                      size_t need_to_read) {
    if (!ec) {
      body_.append(boost::begin(range), size);
      size_t left = need_to_read - size;
      LOG(INFO) << "Read_size: " << size << " Remain_size: " << left;
      if (left > 0) {
        ReadChunk(left, conn);
      } else { // Read Body Done
        LOG(INFO) << "Body: " << body_;               
        LOG(INFO) << "Read Body Done";
      }
    }
  }
};

#define HTTP_METHOD_GEN(x)  \
    x(GET),                 \
    x(POST),                \
    x(OPTIONS),             \
    x(DELETE),              \
    x(HEAD),                \
    x(CONNECT),             \
    x(PUT),                 \
    x(TRACE),               \
    
#define HTTP_METHOD_ENUM(method) method

enum class HTTPMethod {
  HTTP_METHOD_GEN(HTTP_METHOD_ENUM)
};

#undef HTTP_METHOD_ENUM

// ConnectionHandler State
//


// RestAsyncHandler
// 负责与用户交互的界面, 通过它所提供的APIs, 用户可以定义自己的 Route 以及 Handler.
// 它处理了请求的Routing 以及分派 Handler.
//
struct RestAsyncHandler {
  // 回调函数
  void operator() (const RestAsyncServer::request& request, RestAsyncServer::connection_ptr conn) {
    std::shared_ptr<ConnectionHandler> h(new ConnectionHandler(request));
    (*h)(conn);
  }

  // TODO(wqx):
  // APIs
  // void AddBeforeRouteHandler(BeforeRouteHandler before_route_handler);
  // void AddRouter(const std::unique_ptr<Router>& router);
  // void AddAfterRouteHandler(AfterRouteHandler after_route_handler);
  // 
  // private:
  //  void DoRoute(...);
  //  void DoDispatch(...);
  //
};

int main(int argc, char** argv) {
  if (argc != 3) { exit(1); }

  try {
    RestAsyncHandler handler;
    RestAsyncServer::options options(handler);
    RestAsyncServer server(options.address(argv[1]).port(argv[2]));
    server.run();
  }
  catch (std::exception& e) {
    LOG(FATAL) << e.what();
  }

  return 0;
}
