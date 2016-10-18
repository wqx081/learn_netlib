#ifndef SERVER_HTTP_SERVER_H_
#define SERVER_HTTP_SERVER_H_
#include <string>
#include "server/handler_context.h"
#include "server/handler_chain.h"

namespace rest {

class RESTHandler {
 public:

  static RESTHandler* GetInstance();

  // netlib entry point callback
  void operator()(const RESTServer::request& request, 
                  RESTServer::connection_ptr conn);

 private:
  RESTHandler();
};

class HTTPServer {
 public:
  HTTPServer(const std::string& ip, const std::string& port);
  ~HTTPServer();

  void AddPreFilter(Handler* handler);
  void AddPostFilter(Handler* handler); 

  void AddRoute(const std::string& path, Handler* handler); 
  void AddRoute(RoutedHandlerChain::Route route) ; 

  void Run() ; 

 private:
  std::string ip_;
  std::string port_;

  RESTServer::options options_;
  std::unique_ptr<RESTServer> server_;  
};

} // namespace rest
#endif // SERVER_HTTP_SERVER_H_
