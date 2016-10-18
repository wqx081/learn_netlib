#include "server/http_server.h"
#include "server/http_method.h"

#include "server/request_reader.h"
#include "server/response_writer.h"
#include "server/dispatcher.h"

#include <glog/logging.h>

namespace rest {

RESTHandler::RESTHandler() {}

// static
RESTHandler* RESTHandler::GetInstance() {
  static RESTHandler rest_handler_instace;
  return &rest_handler_instace;
}

void RESTHandler::operator()(const RESTServer::request& request,
                             RESTServer::connection_ptr conn) {
  HandlerContext handler_context(request);

  switch (HTTPMethodFromString(request.method)) {
    case HTTPMethod::HEAD: 
      conn->write("HEAD request");
      break;
    case HTTPMethod::GET: 
        conn->write("GET request");
        break;
    case HTTPMethod::PUT:
    case HTTPMethod::POST: 
      {
        auto request_reader = std::make_shared<RequestReader>(handler_context, 
          [](HandlerContext handler_context,
             RESTServer::connection_ptr conn) {
            // 已经完整读取request body, 
            // 创建 Dispatcher， 执行调度
            // LOG(INFO) << "Request_body: " << handler_context.request_body;
            // conn->write("Hello, World");
           std::shared_ptr<Dispatcher> dispatcher = std::make_shared<Dispatcher>(handler_context);
            dispatcher->Schedule(conn);
          });
          conn->read([request_reader](RESTServer::connection::input_range input,
                                      std::error_code ec,
                                      size_t bytes_transferred,
                                      RESTServer::connection_ptr conn) {
            (*request_reader)(input, ec, bytes_transferred, conn);
          });
      }
      break;
    case HTTPMethod::UNKNOWN:
      break;
  }
}

// HTTPServer
HTTPServer::HTTPServer(const std::string& ip, const std::string& port)
      : ip_(ip),
        port_(port),
        options_(*RESTHandler::GetInstance()) {
  server_.reset(new RESTServer(options_.address(ip_).port(port_)));
}

HTTPServer::~HTTPServer() {}

void HTTPServer::AddPreFilter(Handler* handler) {
  PreFilterChain::GetInstance()->AddHandler(handler);
}

void HTTPServer::AddPostFilter(Handler* handler) {
  PostFilterChain::GetInstance()->AddHandler(handler);
}

void HTTPServer::AddRoute(const std::string& path, Handler* handler) {
  RoutedHandlerChain::GetInstance()->AddRoute(path, handler);
}

void HTTPServer::AddRoute(RoutedHandlerChain::Route route) {
  RoutedHandlerChain::GetInstance()->AddRoute(route);
}

void HTTPServer::Run() {
  try {
    server_->run();
  } catch (std::exception& e) {
    LOG(FATAL) << e.what();
  }
}


} // namespace rest

//////////////

class MonitorPreFilter : public rest::Handler {
 public:
  virtual ~MonitorPreFilter() {}
  virtual rest::Status OnHandle(rest::HandlerContext* handler_context) override {
    LOG(INFO) << "MonitorHandler::OnHandler::RequestBody:  " << handler_context->request_body; 
    return rest::Status();
  }
};

class HelloWorldHandler : public rest::Handler {
 public:
  virtual ~HelloWorldHandler() {}
  virtual rest::Status OnHandle(rest::HandlerContext* handler_context) override {
    LOG(INFO) << "HelloWorldHandler::OnHandler::RequestPath:  " << handler_context->request.destination;
    LOG(INFO) << "HelloWorldHandler::OnHandler::RequestBody:  " << handler_context->request_body; 
    handler_context->response_body += "{\"key\":\"Hello, World\"}";
    return rest::Status();
  }
};

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " " << " [ip] [port]" << std::endl;
    return 1;
  }

  rest::HTTPServer http_server(argv[1], argv[2]);

  MonitorPreFilter monitor_pre_filter;
  HelloWorldHandler hello_world_handler;

  http_server.AddPreFilter(&monitor_pre_filter);
  http_server.AddRoute("/hello/", &hello_world_handler);

  http_server.Run();
  return 0;
}
