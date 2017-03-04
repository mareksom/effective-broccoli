#include "makra.h"

#ifndef NDEBUG

debug::~debug() {
  std::lock_guard<std::mutex> guard(mutex_);
  std::cerr << stream_.str() << std::endl;
}

std::mutex debug::mutex_;

#endif
