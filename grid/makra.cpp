#include "makra.h"

#ifndef NDEBUG

namespace Grid {

debug::~debug() {
  std::lock_guard<std::mutex> guard(mutex_);
  std::cerr << stream_.str() << std::endl;
}

std::mutex debug::mutex_;

}  // namespace Grid

#endif
