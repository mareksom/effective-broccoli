#ifndef GRID_RUN_H_
#define GRID_RUN_H_

#include <memory>
#include <thread>

#include "board.h"
#include "controller.h"
#include "options.h"

namespace Grid {

int RunBoard(int argc, char** argv,
             const Options& options, std::unique_ptr<Board> board,
             std::function<void()> user_thread);

}  // namespace Grid

#endif  // GRID_RUN_H_
