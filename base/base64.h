#ifndef BASE_BASE64_H_
#define BASE_BASE64_H_

#include <stdlib.h>
#include <vector>
#include "base/stringpiece.h"

namespace base {

void Base64Encode(StringPiece src, std::string* dest);
bool Base64Decode(StringPiece src, std::string* dest);

} // namespace base
#endif // BASE_BASE64_H_
