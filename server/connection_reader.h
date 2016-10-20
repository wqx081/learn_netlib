#ifndef SERVER_CONNECTION_READER_H_
#define SERVER_CONNECTION_READER_H_

#include "server/handler_context.h"

namespace rest {

class ConnectionReader {
 public:
  virtual ~ConnectionReader() {}

  void operator()(RESTServer::connection::input_range input,
                  std::error_code ec,
                  size_t bytes_transferred,
                  RESTServer::connection_ptr conn) = 0;

};

} // namespace rest
#endif // SERVER_CONNECTION_READER_H_
