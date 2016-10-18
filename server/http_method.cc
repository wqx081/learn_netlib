#include "server/http_method.h"

namespace rest {

HTTPMethod HTTPMethodFromString(const std::string& method) {
  if (method == "HEAD") {
    return HEAD;
  } else if (method == "GET") {
    return GET;
  } else if (method == "PUT") {
    return PUT;
  } else if (method == "POST") {
    return POST;
  } else {
    return UNKNOWN;
  }
}

std::string HTTPMethodToString(HTTPMethod http_method) {
  switch (http_method) {
    case HEAD:  return "HEAD";
    case GET:   return "GET";
    case PUT:   return "PUT";
    case POST:  return "POST";
    default:    return "UNKNOWN";
  }
}

} // namespace rest
