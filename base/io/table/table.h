#ifndef BASE_IO_TABLE_TABLE_H_
#define BASE_IO_TABLE_TABLE_H_

#include <stdint.h>
#include "base/io/table/iterator.h"

namespace base {
class RandomAccessFile;

namespace table {

class Block;
class BlockHandle;
class Footer;
struct Options;

// A Table is a sorted map from strings to strings.  Tables are
// immutable and persistent.  A Table may be safely accessed from
// multiple threads without external synchronization.
class Table {
 public:
  // Attempt to open the table that is stored in bytes [0..file_size)
  // of "file", and read the metadata entries necessary to allow
  // retrieving data from the table.
  //
  // If successful, returns ok and sets "*table" to the newly opened
  // table.  The client should delete "*table" when no longer needed.
  // If there was an error while initializing the table, sets "*table"
  // to NULL and returns a non-ok status.  Does not take ownership of
  // "*file", but the client must ensure that "file" remains live
  // for the duration of the returned table's lifetime.
  static Status Open(const Options& options, RandomAccessFile* file,
                     uint64 file_size, Table** table);

  ~Table();

  // Returns a new iterator over the table contents.
  // The result of NewIterator() is initially invalid (caller must
  // call one of the Seek methods on the iterator before using it).
  Iterator* NewIterator() const;

  // Given a key, return an approximate byte offset in the file where
  // the data for that key begins (or would begin if the key were
  // present in the file).  The returned value is in terms of file
  // bytes, and so includes effects like compression of the underlying data.
  // E.g., the approximate offset of the last key in the table will
  // be close to the file length.
  uint64 ApproximateOffsetOf(const StringPiece& key) const;

 private:
  struct Rep;
  Rep* rep_;

  explicit Table(Rep* rep) { rep_ = rep; }
  static Iterator* BlockReader(void*, const StringPiece&);

  // Calls (*handle_result)(arg, ...) with the entry found after a call
  // to Seek(key).  May not make such a call if filter policy says
  // that key is not present.
  Status InternalGet(const StringPiece& key, void* arg,
                     void (*handle_result)(void* arg, const StringPiece& k,
                                           const StringPiece& v));

  // No copying allowed
  Table(const Table&);
  void operator=(const Table&);
};

}  // namespace table
}  // namespace base

#endif //
