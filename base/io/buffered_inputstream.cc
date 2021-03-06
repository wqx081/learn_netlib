#include "base/io/buffered_inputstream.h"
#include "base/io/random_inputstream.h"

namespace base {
namespace io {

BufferedInputStream::BufferedInputStream(InputStreamInterface* input_stream,
                                         size_t buffer_size,
                                         bool owns_input_stream)
    : input_stream_(input_stream),
      size_(buffer_size),
      owns_input_stream_(owns_input_stream) {
  buf_.reserve(size_);
}

BufferedInputStream::BufferedInputStream(RandomAccessFile* file,
                                         size_t buffer_size)
    : BufferedInputStream(new RandomAccessInputStream(file), buffer_size,
                          true) {}

BufferedInputStream::~BufferedInputStream() {
  if (owns_input_stream_) {
    delete input_stream_;
  }
}

Status BufferedInputStream::FillBuffer() {
  Status s = input_stream_->ReadNBytes(size_, &buf_);
  pos_ = 0;
  limit_ = buf_.size();
  return s;
}

Status BufferedInputStream::ReadLineHelper(string* result, bool include_eol) {
  result->clear();
  Status s;
  while (true) {
    if (pos_ == limit_) {
      // Get more data into buffer
      s = FillBuffer();
      if (limit_ == 0) {
        break;
      }
    }
    char c = buf_[pos_++];
    if (c == '\n') {
      if (include_eol) {
        *result += c;
      }
      return Status::OK();
    }
    // We don't append '\r' to *result
    if (c != '\r') {
      *result += c;
    }
  }
  if (errors::IsOutOfRange(s) && !result->empty()) {
    return Status::OK();
  }
  return s;
}

Status BufferedInputStream::ReadNBytes(int64 bytes_to_read, string* result) {
  if (bytes_to_read < 0) {
    return errors::InvalidArgument("Can't read a negative number of bytes: ",
                                   bytes_to_read);
  }
  result->clear();
  result->reserve(bytes_to_read);

  Status s;
  while (result->size() < static_cast<size_t>(bytes_to_read)) {
    // Check whether the buffer is fully read or not.
    if (pos_ == limit_) {
      s = FillBuffer();
      // If we didn't read any bytes, we're at the end of the file; break out.
      if (limit_ == 0) {
        break;
      }
    }
    const int64 bytes_to_copy =
        std::min<int64>(limit_ - pos_, bytes_to_read - result->size());
    result->insert(result->size(), buf_, pos_, bytes_to_copy);
    pos_ += bytes_to_copy;
  }
  // Filling the buffer might lead to a situation when we go past the end of
  // the file leading to an OutOfRange() status return. But we might have
  // obtained enough data to satisfy the function call. Returning OK then.
  if (errors::IsOutOfRange(s) &&
      (result->size() == static_cast<size_t>(bytes_to_read))) {
    return Status::OK();
  }
  return s;
}

Status BufferedInputStream::SkipNBytes(int64 bytes_to_skip) {
  if (bytes_to_skip < 0) {
    return errors::InvalidArgument("Can only skip forward, not ",
                                   bytes_to_skip);
  }
  if (pos_ + bytes_to_skip < limit_) {
    // If we aren't skipping too much, then we can just move pos_;
    pos_ += bytes_to_skip;
  } else {
    // Otherwise, we already have read limit_ - pos_, so skip the rest. At this
    // point we need to get fresh data into the buffer, so reset pos_ and
    // limit_.
    Status s = input_stream_->SkipNBytes(bytes_to_skip - (limit_ - pos_));
    pos_ = 0;
    limit_ = 0;
    return s;
  }
  return Status::OK();
}

int64 BufferedInputStream::Tell() const {
  return input_stream_->Tell() - (limit_ - pos_);
}

Status BufferedInputStream::Seek(int64 position) {
  if (position < 0) {
    return errors::InvalidArgument("Seeking to a negative position: ",
                                   position);
  }

  // Position of the buffer within file.
  const int64 bufpos = Tell();
  if (position < bufpos) {
    // Reset input stream and skip 'position' bytes.
    RETURN_IF_ERROR(Reset());
    return SkipNBytes(position);
  }

  return SkipNBytes(position - bufpos);
}

Status BufferedInputStream::Reset() {
  RETURN_IF_ERROR(input_stream_->Reset());
  pos_ = 0;
  limit_ = 0;
  return Status::OK();
}

Status BufferedInputStream::ReadLine(string* result) {
  return ReadLineHelper(result, false);
}

string BufferedInputStream::ReadLineAsString() {
  string result;
  ReadLineHelper(&result, true);
  return result;
}

}  // namespace io
}  // namespace base
