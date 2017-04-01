#ifndef RUN_H_
#define RUN_H_

#include <memory>

#include "board.h"
#include "controller.h"
#include "options.h"

int RunBoard(int argc, char** argv,
             Options options, std::unique_ptr<Board> board);

#endif  // RUN_H_
