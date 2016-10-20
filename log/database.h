#ifndef LOG_DATABASE_H_
#define LOG_DATABASE_H_
#include <memory>
#include <string>

#include "base/macros.h"
#include "log/logged_entry.h"

namespace log {


class Database {
 public:
  enum LookupResult {
    LOOKUP_OK,
    NOT_FOUND,
  };

  enum WriteResult {
    OK,
    MISSING_CHARGE_HASH,
    DUPLICATE_CHARGE_HAS,
    ENTRY_NOT_FOUND,
    SEQUENCE_NUMBER_ALREADY_IN_USE,
  };

  class Iterator {
   public:
    Iterator() = default;
    virtual ~Iterator() = default;

    virtual bool GetNextEntry(LoggedEntry* entry) = 0;
   private:
    DISALLOW_COPY_AND_ASSIGN(Iterator);
  };

  virtual ~Database() = default;

  // For Read
  virtual LookupResult LookupByHash(const std::string& hash, LoggedEntry* result) const = 0;
  virtual LookupResult LookupByIndex(int64_t sequence_number, LoggedEntry* result) const = 0;
  virtual std::unique_ptr<Iterator> ScanEntries(int64_t start_index) const = 0;

  // For Write
  virtual WriteResult CreateSequencedEntry(const LoggedEntry&) = 0;

 protected:
  Database() = default;
  
 private:
  DISALLOW_COPY_AND_ASSIGN(Database);
};

} // namespace log
#endif // LOG_DATABASE_H_
