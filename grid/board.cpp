#include "board.h"

namespace Grid {

Board::Board() : options_(nullptr) {}

void Board::SetOptions(const Options* options) {
  options_ = options;
}

const Options& Board::options() const {
  return *options_;
}

}  // namespace Grid
