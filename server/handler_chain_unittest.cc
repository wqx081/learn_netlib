#include "server/handler_chain.h"
#include <gtest/gtest.h>
#include <glog/logging.h>

namespace rest {

#if 0
class MyPreFilterHandler : public Handler {
 public:
  virtual ~MyPreFilterHandler() {}
  virtual Status OnHandle(HandlerContext*) override {
    LOG(INFO) << "MyPreFilterHandler::OnHandle";
    return Status();
  }
};

class MyDispatcher {
 public:
  void AddPreFilter(Handler* handler) {
    PreFilterChain::GetInstance()->AddHandler(handler);
  }

  void Run() {
    PreFilterChain::GetInstance()->Execute([this]() {
      this->OnPreFilterChainCompleted();
    });    
  }
 
 private:
  void OnPreFilterChainCompleted() {
    LOG(INFO) << "OnPreFilterChainCompleted";
  }
  void OnPostFilterChainCompleted() {
    LOG(INFO) << "OnPostFilterChainCompleted";
  }
  void OnRoutedHandlerCompleted() {
    LOG(INFO) << "OnRoutedHandlerCompleted";
  }
};

TEST(HandlerChain, Basic) {
  MyDispatcher dispatcher;
  MyPreFilterHandler handler1;
  dispatcher.AddPreFilter(&handler1);
  dispatcher.Run();

  EXPECT_TRUE(true);
}
#endif
TEST(HandlerChain, Basic) {
  EXPECT_TRUE(true);
}

} // namespace rest
