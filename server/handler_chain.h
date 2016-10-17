#ifndef SERVER_HANDLER_CHAIN_H_
#define SERVER_HANDLER_CHAIN_H_
#include <memory>
#include <vector>
#include <algorithm>
#include <iterator>

#include "server/handler_context.h"

namespace rest {

class Handler {
 public:
  virtual ~Handler() {}
  virtual Status OnHandle(HandlerContext* handler_context) = 0;  
};

template<typename H,
         typename Container = std::vector<H>> 
class HandlerChain {
 public:
  typedef typename Container::value_type value_type;
  typedef typename Container::size_type  size_type;
  //typedef const value_type* const_iterator;
  typedef typename Container::iterator const_iterator;

  class Watcher {
   public:
    virtual ~Watcher() {}
    virtual void OnComplete() = 0;
  };
  
  virtual ~HandlerChain() {}

  virtual void Execute() = 0;
  virtual void AddHandler(const value_type& x) {
    for (auto& h : container_) {
      if (h == x) {
        return;
      }
    }  
    container_.insert(container_.begin(), x);
  }
  
  const_iterator begin() const { return container_.begin(); }
  const_iterator end() const { return container_.end(); }
  size_type size() const { return container_.size(); }

  void RegisterWatcher(Watcher* watcher) {
    auto it = std::find(watcher_list_.begin(), watcher_list_.end());
    if (it != watcher_list_.end()) {
      return;
    }
    watcher_list_.push_back(watcher);
  }

  void UnregisterWatcher(Watcher* watcher) {
    auto it = std::find(watcher_list_.begin(), watcher_list_.end());
    if (it != watcher_list_.end()) {
      watcher_list_.erase(it);
    } 
  }

  void IterateOnCompleteWatch() {
    for (const auto& watcher : watcher_list_) {
      if (watcher) {
        watcher->OnComplete();
      }
    }
  }
 private:
  Container container_;
  std::vector<Watcher*> watcher_list_;
};

class PreFilterChain : public HandlerChain<Handler*> {
 public:
  virtual void Execute() override {
    if (size() > 0) {
      HandlerChain<Handler*>::const_iterator it = begin();
      HandlerChain<Handler*>::const_iterator e = end();
      for (; it != e; ++it) {
        if (*it) {
          (*it)->OnHandle(nullptr);
        }
      }
      // Notify Watcher
      IterateOnCompleteWatch();
    }
  }
};


} // namespace rest
#endif // SERVER_HANDLER_CHAIN_H_
