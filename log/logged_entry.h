#ifndef LOG_LOGGED_ENTRY_H_
#define LOG_LOGGED_ENTRY_H_
#include <string>

namespace log {

//TODO(wqx):
class LoggedEntry {
 public:
  // APIS
  bool SerializeToString(std::string*) const { return true; }
  int64_t sequence_number() const { return 11; }
  std::string Hash() const { return "Hash"; }
  bool ParseFromString(const std::string&) { return true; }

};

} // namespace log
#endif // LOG_LOGGED_ENTRY_H_
