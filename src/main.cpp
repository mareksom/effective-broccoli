#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include "grid/controller.h"
#include "grid/hex_board.h"
#include "grid/makra.h"
#include "grid/object.h"
#include "grid/options.h"
#include "grid/painter.h"
#include "grid/run.h"
#include "grid/square_board.h"
#include "grid/viewer.h"
#include "makra.h"

std::mutex position_mutex;
int px, py;

struct Contr : Grid::Controller {
  void Init() {
    AddSingleMessageBox(
        1, 0, 0, 1,
        [&](Grid::StreamReader& sr) -> void {
          std::lock_guard<std::mutex> lock(position_mutex);
          sr << "Position: (" << px << ", " << py << ")";
        });
    AddSingleMessageBox(
        1, 1, 1, 1,
        [&](Grid::StreamReader& sr) -> void {
          std::lock_guard<std::mutex> lock(position_mutex);
          sr << rand() << " <- random value";
        });
  }

  int BoardWidth() override {
    return 100;
  }

  int BoardHeight() override {
    return 100;
  }

  int GetFieldColor(int x, int y) override {
    srand((50 * 1000 * x + y) ^ 0xfffff);
    const int r = rand() % 256;
    const int g = rand() % 256;
    const int b = rand() % 256;
    return Grid::MakeColor(r, g, b);
  }

  int GetObject(int x, int y) override {
    srand(50 * 1000 * x + y);
    const int r = rand() % 256;
    const int g = rand() % 256;
    const int b = rand() % 256;
    position_mutex.lock();
    const int dx = px;
    const int dy = py;
    position_mutex.unlock();
    if (x == dx and y == dy) {
      return Grid::MakeObject(Grid::Object::kSad, r, g, b);
    } else {
      return Grid::MakeObject(Grid::Object::kNone, r, g, b);
    }
  }

  void FieldClick(int x, int y, int button) override {
    if (button == 1) {
      debug() << "Lewy" << imie(x) << imie(y);
    } else if (button == 2) {
      debug() << "Srodkowy" << imie(x) << imie(y);
    } else if (button == 3) {
      debug() << "Prawy" << imie(x) << imie(y);
    } else {
      debug() << "Dziwny" << imie(x) << imie(y);
    }
  }

  void KeyPress(const std::string& button) override {
    int dx = 0, dy = 0;
    if (button == "left") {
      dx = -1;
    } else if (button == "right") {
      dx = 1;
    } else if (button == "up") {
      dy = -1;
    } else if (button == "down") {
      dy = 1;
    } else {
      debug() << "KeyPress(" << imie(button) << ")";
    }
    if (dx != 0 or dy != 0) {
      position_mutex.lock();
      const int old_x = px;
      const int old_y = py;
      const int new_x = old_x + dx;
      const int new_y = old_y + dy;
      if (0 <= new_x and new_x < BoardWidth() and
          0 <= new_y and new_y < BoardHeight()) {
        px = new_x;
        py = new_y;
        position_mutex.unlock();
        InvalidateField(old_x, old_y);
        InvalidateField(new_x, new_y);
        CenterOn(new_x, new_y);
        AddMessage() << "Centering on: (" << new_x << ", " << new_y << ") \n  |----> Wciecie\n  `-----> Drugie.";
      } else {
        position_mutex.unlock();
      }
    }
  }
} controller;

int main(int argc, char** argv) {
  controller.Init();

  Grid::Options options;
  options.SetController(&controller);

  std::string board_type;
  while (board_type.empty()) {
    std::cout << "Select the type of the board: [square/hex]:" << std::endl;
    std::cin >> board_type;
    if (!board_type.empty()) {
      if (board_type[0] == 's' or board_type[0] == 'S') {
        board_type = "square";
      } else if (board_type[0] == 'h' or board_type[0] == 'H') {
        board_type = "hex";
      } else {
        std::cout << "Unrecognized type: '" << board_type << "'." << std::endl;
        board_type.clear();
        continue;
      }
      break;
    }
  }

  std::unique_ptr<Grid::Board> board;
  if (board_type == "square") {
    board = std::make_unique<Grid::SquareBoard>();
  } else if (board_type == "hex") {
    board = std::make_unique<Grid::HexBoard>();
  } else {
    assert(false);
  }
  return Grid::RunBoard(argc, argv, options, std::move(board));
}
