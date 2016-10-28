#include "base/stringpiece.h"
#include "base/strings/strcat.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <glog/logging.h>
#include <gtest/gtest.h>

namespace base {
namespace libevent {

TEST(HTTPClient, Sync) {

  const StringPiece query = strings::StrCat("GET / HTTP/1.1\r\n",
                                            "Host: www.baidu.com\r\n",
                                            "\r\n");
  const StringPiece hostname("www.baidu.com");

  EXPECT_TRUE(true);
}

} // namespace event
} // namespace base
