#ifndef LOG_LEVELDB_DATABASE_H_
#define LOG_LEVELDB_DATABASE_H_

#include "log/database.h"

#include <leveldb/db.h>
#include <leveldb/filter_policy.h>

#include <stdint.h>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <unordered_map>
#include <vector>

#include "base/macros.h"
#include "log/logged_entry.h"


namespace log {

class LeveldbDatabase : public Database {
 public:
  explicit LeveldbDatabase(const std::string& db_file);
  virtual ~LeveldbDatabase() = default;

  virtual LookupResult LookupByHash(const std::string& hash, LoggedEntry* result) const override;
  virtual LookupResult LookupByIndex(int64_t sequence_number, LoggedEntry* result) const override;
  virtual std::unique_ptr<Database::Iterator> ScanEntries(int64_t start_index) const override;
  virtual WriteResult CreateSequencedEntry(const LoggedEntry& logged) override;

 private:
  class Iterator;

  void BuildIndex();
  void InsertEntryMapping(int64_t sequence_number, const std::string& hash);

  mutable std::mutex lock_;
  const std::unique_ptr<const leveldb::FilterPolicy> filter_policy_;
  std::unique_ptr<leveldb::DB> db_;

  int64_t contiguous_size_;
  std::unordered_map<std::string, int64_t> id_by_hash_;

  std::set<int64_t> sparse_entries_;

  DISALLOW_COPY_AND_ASSIGN(LeveldbDatabase);
};

} // namespace log
#endif // LOG_LEVELDB_DATABASE_H_
