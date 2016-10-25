#ifndef SERVER_HANDLER_CHAIN_H_
#define SERVER_HANDLER_CHAIN_H_
#include <memory>
#include <vector>
#include <algorithm>
#include <iterator>

#include <mutex>
#include <functional>
#include <utility>

#include "base/gtl/map_util.h"
#include "server/handler_context.h"

namespace rest {

class Handler {
 public:
  virtual ~Handler() {}
  virtual Status OnHandle(HandlerContext* handler_context) = 0;  
};

template<typename H,
         typename Container = std::vector<H>> 
class FilterChain {
 public:
  typedef typename Container::value_type value_type;
  typedef typename Container::size_type  size_type;
  typedef typename Container::iterator const_iterator;
  
  virtual ~FilterChain() {}

  virtual void Execute(HandlerContext* handler_context, std::function<void()> completed_callback) {
    std::unique_lock<std::mutex> l(mu_);

    if (size() > 0) {
      const_iterator it = begin();
      const_iterator e = end();
      while (it != e) {
        if (*it) {
          (*it)->OnHandle(handler_context);
        }
        it++;
      }
      if (completed_callback) {
        completed_callback();
      }
    }
  }

  virtual void AddHandler(const value_type& x) {
    std::unique_lock<std::mutex> l(mu_);

    for (auto& h : container_) {
      if (h == x) {
        return;
      }
    }  
    container_.insert(container_.begin(), x);
  }
  
  const_iterator begin() { return container_.begin(); }
  const_iterator end() { return container_.end(); }
  size_type size() const { return container_.size(); }

 protected:
  FilterChain() {}

 private:

  FilterChain(const FilterChain&);
  void operator=(const FilterChain&);

  Container container_;
  std::mutex mu_;
};

////////////// Filter Chain
//
class PreFilterChain : public FilterChain<Handler*> {
 public:
  static PreFilterChain* GetInstance() {
    static PreFilterChain pre_filter_chain_instance;
    return &pre_filter_chain_instance;
  }

 private:
  PreFilterChain() {}
};

class PostFilterChain : public FilterChain<Handler*> {
 public:
  static PostFilterChain* GetInstance() {
    static PostFilterChain post_filter_chain_instance;
    return &post_filter_chain_instance;
  }

 private:
  PostFilterChain() {}
};

//////// Routed Handler Chain
//
class RoutedHandlerChain {
 public:
  typedef std::pair<std::string, Handler*> Route;
  
  static RoutedHandlerChain* GetInstance() {
    static RoutedHandlerChain instance;
    return &instance;
  }

  void AddRoute(const std::string& path, Handler* handler) {
    std::unique_lock<std::mutex> l(mu_);

    base::gtl::InsertIfNotPresent(&route_map_, path, handler);
  }

  void AddRoute(std::pair<std::string, Handler*> route) {
    std::unique_lock<std::mutex> l(mu_);

    base::gtl::InsertIfNotPresent(&route_map_, route);
  }

  Status RoutingAndExecute(const std::string& path, 
                           HandlerContext* handler_context,
                           std::function<void()> completed_callback) {
    std::unique_lock<std::mutex> l(mu_);

    auto handler = base::gtl::FindPtrOrNull(route_map_, path);
    if (handler) {
      handler->OnHandle(handler_context);
      if (completed_callback) {
        completed_callback();
      }
      return Status();
    }
    return Status();
  }

 private:
  RoutedHandlerChain() {}

  RoutedHandlerChain(const RoutedHandlerChain&);
  void operator=(const RoutedHandlerChain&);

  std::map<std::string, Handler*> route_map_;  
  std::mutex mu_;
};

} // namespace rest
#endif // SERVER_HANDLER_CHAIN_H_
