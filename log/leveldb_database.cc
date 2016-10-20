#include "log/leveldb_database.h"
#include "base/string_encode.h"

#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_int32(leveldb_max_open_files, 0,
             "number of open files that can be used by leveldb");

DEFINE_int32(leveldb_bloom_filter_bits_per_key, 0,
             "number of open files that can be used by leveldb");

namespace log {
namespace {

const char kEntryPrefix[] = "entry-";

std::unique_ptr<const leveldb::FilterPolicy> BuildFilterPolicy() {
  std::unique_ptr<const leveldb::FilterPolicy> retval;
  
  if (FLAGS_leveldb_bloom_filter_bits_per_key > 0) {
    retval.reset(CHECK_NOTNULL(leveldb::NewBloomFilterPolicy(
      FLAGS_leveldb_bloom_filter_bits_per_key)));
  }

  return retval;
}

std::string IndexToKey(int64_t index) {
  const char nibble[] = "0123456789abcdef";
  std::string index_str(sizeof(index) * 2, nibble[0]);
  for (int i = sizeof(index) * 2; i > 0 && index > 0; --i) {
    index_str[i - 1] = nibble[index & 0xf];
    index = index >> 4;
  }
  
  return kEntryPrefix + index_str;
}

int64_t KeyToIndex(leveldb::Slice key) {
  CHECK(key.starts_with(kEntryPrefix));
  key.remove_prefix(strlen(kEntryPrefix));
  const std::string index_str(base::HexDecode(key.ToString()));

  int64_t index(0);
  CHECK_EQ(index_str.size(), sizeof(index));
  for (size_t i = 0; i < sizeof(index); ++i) {
    index = (index << 8) | static_cast<unsigned char>(index_str[i]);
  }
  return index;
}

} // namespace

class LeveldbDatabase::Iterator : public Database::Iterator {
 public:
  Iterator(const LeveldbDatabase* db, int64_t start_index)
      : it_(db->db_->NewIterator(leveldb::ReadOptions())) {
    CHECK(it_);
    it_->Seek(IndexToKey(start_index));
  }

  virtual bool GetNextEntry(LoggedEntry* entry) override {
    if (!it_->Valid() || !it_->key().starts_with(kEntryPrefix)) {
      return false;
    }

    const int64_t seq(KeyToIndex(it_->key()));
    CHECK(entry->ParseFromString(std::string(it_->value().data(), it_->value().size())))
          << "failed to parse entry for key " << it_->key().ToString();
    //CHECK(entry->has_sequence_number())
    //      << "no sequence number for entry with expected sequence number "
     //     << seq;
      CHECK_EQ(entry->sequence_number(), seq) << "unexpected sequence_number";

    it_->Next();

    return true;
  }
 private:
  const std::unique_ptr<leveldb::Iterator> it_;
};

LeveldbDatabase::LeveldbDatabase(const std::string& db_file)
    : filter_policy_(BuildFilterPolicy()),
      contiguous_size_(0) {
  LOG(INFO) << "Opening  " << db_file;

  leveldb::Options options;
  options.create_if_missing = true;
  options.filter_policy = filter_policy_.get();

  leveldb::DB* db;
  leveldb::Status status = leveldb::DB::Open(options, db_file, &db);
  CHECK(status.ok()) << status.ToString();
  db_.reset(db);

  BuildIndex();
}

Database::WriteResult LeveldbDatabase::CreateSequencedEntry(const LoggedEntry& logged) {
  std::unique_lock<std::mutex> l(lock_);
  std::string data;
  logged.SerializeToString(&data);
  std::string key(IndexToKey(logged.sequence_number()));

  std::string existing_data;
  leveldb::Status status = db_->Get(leveldb::ReadOptions(), key, &existing_data);
  if (status.IsNotFound()) {
    status = db_->Put(leveldb::WriteOptions(), key, data);
    CHECK(status.ok()) << "Failed to write sequenced entry (seq: "
                       << logged.sequence_number()
                       << "): " << status.ToString();
  } else {
    if (existing_data == data) {
      return this->OK;
    }
    return this->SEQUENCE_NUMBER_ALREADY_IN_USE;
  }

  InsertEntryMapping(logged.sequence_number(), logged.Hash());

  return this->OK;
}

Database::LookupResult LeveldbDatabase::LookupByHash(const std::string& hash,
                                                     LoggedEntry* result) const {
  std::unique_lock<std::mutex> l(lock_);
  auto i(id_by_hash_.find(hash));
  if (i == id_by_hash_.end()) {
    return NOT_FOUND;
  }

  std::string data;
  const leveldb::Status status = db_->Get(leveldb::ReadOptions(), IndexToKey(i->second), &data);  
  if (status.IsNotFound()) {
    return NOT_FOUND;
  }
  CHECK(status.ok()) << "Failed to get entry by hash (" << base::HexEncode(hash) << "): " << status.ToString();

  if (result) {
    CHECK(result->ParseFromString(data));
    CHECK_EQ(result->Hash(), hash);
  }

  return LOOKUP_OK;
}

Database::LookupResult LeveldbDatabase::LookupByIndex(int64_t sequence_number,
                                                      LoggedEntry* result) const {
  std::string data;
  leveldb::Status status = db_->Get(leveldb::ReadOptions(), 
                                    IndexToKey(sequence_number),
                                    &data);
  if (status.IsNotFound()) {
    return NOT_FOUND;
  }

  CHECK(status.ok()) << "Failed to get entry for sequence number: " << sequence_number;

  if (result) {
    CHECK(result->ParseFromString(data));
    CHECK_EQ(result->sequence_number(), sequence_number);
  }

  return LOOKUP_OK;
}

std::unique_ptr<Database::Iterator> LeveldbDatabase::ScanEntries( int64_t start_index) const {
  return std::unique_ptr<Iterator>(new Iterator(this, start_index));
}

void LeveldbDatabase::BuildIndex() {
  std::lock_guard<std::mutex> l(lock_);

  leveldb::ReadOptions options;
  options.fill_cache = false;
  std::unique_ptr<leveldb::Iterator> it(db_->NewIterator(options));
  CHECK(it);
  it->Seek(kEntryPrefix);

  for (; it->Valid() && it->key().starts_with(kEntryPrefix); it->Next()) {
    const int64_t seq(KeyToIndex(it->key()));
    LoggedEntry logged;
    CHECK(logged.ParseFromString(it->value().ToString()))
          << "Failed to parse entry with sequence number " << seq;
    //CHECK(logged.has_sequence_number())
    //      << "No sequence number for entry with sequence number " << seq;
    CHECK_EQ(logged.sequence_number(), seq)
          << "Entry has unexpected sequence_number: " << seq;
  
    InsertEntryMapping(logged.sequence_number(), logged.Hash());
  }
}

void LeveldbDatabase::InsertEntryMapping(int64_t sequence_number, const std::string& hash) {
  if (!id_by_hash_.insert(std::make_pair(hash, sequence_number)).second) {
    id_by_hash_[hash] = std::min(id_by_hash_[hash], sequence_number);
  }

  if (sequence_number == contiguous_size_) {
    ++contiguous_size_;
    for (auto i = sparse_entries_.find(contiguous_size_);
         i != sparse_entries_.end() && *i == contiguous_size_;) {
      ++contiguous_size_;
      i = sparse_entries_.erase(i);
    }
  } else {
    CHECK(sparse_entries_.insert(sequence_number).second)
     << "sequence number " << sequence_number << " already assigned.";
  }
}

} // namespace log
