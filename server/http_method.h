#ifndef SERVER_HTTP_METHOD_H_
#define SERVER_HTTP_METHOD_H_
#include <string>

namespace rest {

enum HTTPMethod {
  UNKNOWN = 0,
  HEAD,
  GET,
  PUT,
  POST,
};

HTTPMethod HTTPMethodFromString(const std::string& method);
std::string HTTPMethodToString(HTTPMethod http_method);

} // namespace rest
#endif // SERVER_HTTP_METHOD_H_
