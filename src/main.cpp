#include <boost/gil/extension/io/png_dynamic_io.hpp>
#include <boost/gil/gil_all.hpp>

namespace gil = boost::gil;

#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <iostream>
#include <string>
#include <thread>

#include "controller.h"
#include "hex_board.h"
#include "makra.h"
#include "object.h"
#include "options.h"
#include "painter.h"
#include "run.h"
#include "square_board.h"
#include "viewer.h"

struct Contr : Controller {
  gil::rgb8_image_t input;

  void Init() {
    gil::png_read_and_convert_image("obrazek.png", input);
    debug() << imie(BoardWidth()) << imie(BoardHeight());
  }

  int BoardWidth() override {
    return input.width();
  }

  int BoardHeight() override {
    return input.height();
  }

  int GetFieldColor(int x, int y) override {
    gil::rgb8_pixel_t pixel = view(input)(x, y);
    return MakeColor(pixel[0], pixel[1], pixel[2]);
  }

  int GetObject(int x, int y) override {
    srand(50 * 1000 * x + y);
    const int wybor = rand() % static_cast<int>(Object::kCount);
    const int r = rand() % 256;
    const int g = rand() % 256;
    const int b = rand() % 256;
    return MakeObject(static_cast<Object>(wybor), r, g, b);
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
    debug() << "KeyPress(" << imie(button) << ")";
  }
} controller;

int main(int argc, char** argv) {
  controller.Init();

  Options options;
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

  std::unique_ptr<Board> board;
  if (board_type == "square") {
    board = std::make_unique<SquareBoard>();
  } else if (board_type == "hex") {
    board = std::make_unique<HexBoard>();
  } else {
    assert(false);
  }
  return RunBoard(argc, argv, options, std::move(board));
}
