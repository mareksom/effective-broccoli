#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include "communication/connect.h"
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

struct Contr : Grid::Controller {
  void Init() {}

  int BoardWidth() override {
    return 100;
  }

  int BoardHeight() override {
    return 100;
  }

  int GetFieldColor(int x, int y) override {
    return Grid::MakeColor(255, 255, 255);
  }

  int GetObject(int x, int y) override {
    return Grid::MakeObject(Grid::Object::kNone, 0, 0, 0);
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

void Algorytm() {
  Connect("test.natodia.net", 10000);
  Disconnect();
}

int main(int argc, char** argv) {
  controller.Init();
  Grid::Options options;
  options.SetController(&controller);

  std::thread(
      []() -> void {
        Algorytm();
      }).detach();

  return Grid::RunBoard(argc, argv, options,
                        std::make_unique<Grid::SquareBoard>());
}
