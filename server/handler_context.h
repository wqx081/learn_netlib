#ifndef SERVER_HANDLER_CONTEXT_H_
#define SERVER_HANDLER_CONTEXT_H_
#include <vector>
#include <boost/network/protocol/http/server.hpp>

namespace rest {

namespace http = boost::network::http;

class RESTHandler;
typedef http::server<RESTHandler> RESTServer;

struct HandlerContext {
  HandlerContext(RESTServer::request req)
    : request(std::move(req)) {}

  RESTServer::request request;
  std::string request_body;

  std::vector<RESTServer::response_header> response_headers;
  std::string response_body; 
};

class Status {
 public:
  Status() {}
};

} // namespace rest
#endif // SERVER_HANDLER_CONTEXT_H_
