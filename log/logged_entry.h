#ifndef LOG_LOGGED_ENTRY_H_
#define LOG_LOGGED_ENTRY_H_
#include <string>

namespace log {

//TODO(wqx):
class LoggedEntry {
 public:
  // APIS
  bool SerializeToString(std::string*) const;
  int64_t sequence_number() const;
  std::string Hash() const;
  bool ParseFromString(const std::string&);

};

} // namespace log
#endif // LOG_LOGGED_ENTRY_H_
