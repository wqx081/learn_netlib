#include "base/macros.h"
#include "base/port.h"
#include "base/logging.h"

#include <event2/event.h>
#include <evhtp.h>

#include <gtest/gtest.h>

#include <memory>

namespace base {
namespace http {

struct Options {
  int num_threads = 0;
  std::string bind_address;
  int bind_port;
  std::string ssl_pem;
  std::string ssl_ca;
  std::string bandwidth_limit;
  int max_keepalives = 1024;

  static Options Default() {
    static Options options;
    options.num_threads = 0;
    options.bind_address = "0.0.0.0";
    options.bind_port = 9980;
    options.ssl_pem = "";
    options.ssl_ca = "";
    return options;
  }
};


void GenCallback(evhtp_request_t* request, void* arg) {
  (void) arg;
  LOG(INFO) << "GlobCallback";

  evbuffer_add(request->buffer_out, "test_glob_cb\n", 13);
  evhtp_send_reply(request, EVHTP_RES_OK);
}

class HTTPServer {
 public:
  explicit HTTPServer(const Options& options);
    

  void Start();
  void Stop();

 private:
  void Init() {
    ev_base_ = event_base_new();
    htp_ = evhtp_new(ev_base_, nullptr);
    DCHECK(ev_base_);
    DCHECK(htp_);
    evhtp_set_gencb(htp_, GenCallback, nullptr);
  }

  const Options& options_;

  evbase_t* ev_base_;
  evhtp_t*  htp_;
  
  DISALLOW_COPY_AND_ASSIGN(HTTPServer);
};


HTTPServer::HTTPServer(const Options& options)
    : options_(options) {
  Init();
}

void HTTPServer::Start() {
  LOG(INFO) << "Server Run at: " << options_.bind_address << ": " << options_.bind_port;
  evhtp_bind_socket(htp_, 
                    options_.bind_address.c_str(), 
                    options_.bind_port, 2046);
  // TODO(wqx): Add signal handler
  event_base_loop(ev_base_, 0);
}

void HTTPServer::Stop() {
}


TEST(HTTPServer, Basic) {
  HTTPServer server(Options::Default());
  server.Start();
}

} // namespace http
} // namespace base
