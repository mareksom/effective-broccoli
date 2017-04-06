#ifndef GRID_RUN_H_
#define GRID_RUN_H_

#include <memory>

#include "board.h"
#include "controller.h"
#include "options.h"

namespace Grid {

int RunBoard(int argc, char** argv,
             Options options, std::unique_ptr<Board> board);

}  // namespace Grid

#endif  // GRID_RUN_H_
