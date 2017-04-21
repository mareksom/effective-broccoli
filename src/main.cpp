#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include "communication/communication.h"
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

Grid::Controller* controller;

enum class TypPola {
  kUnknown,
  kMe, kStart, kEnd, kWall, kEmpty
};

map<pair<int, int>, TypPola> zajete;

TypPola pole(int x, int y) {
  auto it = zajete.find(make_pair(x, y));
  if (it == zajete.end()) return TypPola::kUnknown;
  return it->second;
}

void UstawPole(int x, int y, TypPola typ) {
  zajete[make_pair(x, y)] = typ;
  switch (typ) {
    case TypPola::kUnknown:
      controller->SetFieldColor(x, y, 128, 128, 128);
      controller->SetObject(x, y, Grid::Object::kNone, 0, 0, 0);
      controller->SetText(x, y) << "Unknown (" << x << ", " << y << ")";
      break;

    case TypPola::kMe:
      controller->SetFieldColor(x, y, 255, 255, 255);
      controller->SetObject(x, y, Grid::Object::kSad, 0, 255, 0);
      controller->SetText(x, y) << "(" << x << ", " << y << ")";
      break;

    case TypPola::kStart:
      controller->SetFieldColor(x, y, 255, 216, 0);
      controller->SetObject(x, y, Grid::Object::kSquare, 0, 0, 255);
      controller->SetText(x, y) << "Start (" << x << ", " << y << ")";
      break;

    case TypPola::kEnd:
      controller->SetFieldColor(x, y, 255, 0, 0);
      controller->SetObject(x, y, Grid::Object::kFlag, 0, 255, 0);
      controller->SetText(x, y) << "End (" << x << ", " << y << ")";
      break;

    case TypPola::kWall:
      controller->SetFieldColor(x, y, 0, 0, 0);
      controller->SetObject(x, y, Grid::Object::kNone, 0, 0, 0);
      controller->SetText(x, y) << "Wall";
      break;

    case TypPola::kEmpty:
      controller->SetFieldColor(x, y, 255, 255, 255);
      controller->SetObject(x, y, Grid::Object::kNone, 0, 0, 0);
      controller->SetText(x, y) << "Empty (" << x << ", " << y << ")";
      break;

    deafult:
      debug() << "Something's wrong :(";
  }
}

int my_x, my_y;

mutex dmut;
int dx, dy;

void OnKeyPress(const string& key) {
  int loc_dx = 0, loc_dy = 0;
  if (key == "left") {
    loc_dx = -1;
  } else if (key == "right") {
    loc_dx = 1;
  } else if (key == "up") {
    loc_dy = -1;
  } else if (key == "down") {
    loc_dy = 1;
  } else {
    return;
  }
  lock_guard<mutex> lock(dmut);
  dx = loc_dx;
  dy = loc_dy;
}

void TryMove() {
  int loc_dx, loc_dy;
  /* Lock */ {
    lock_guard<mutex> lock(dmut);
    loc_dx = dx;
    loc_dy = dy;
    dx = 0;
    dy = 0;
  }
  if (loc_dx or loc_dy) {
    out("MOVE %d %d\n", loc_dx, loc_dy);
    if (expectOK()) {
      my_x += loc_dx;
      my_y += loc_dy;
      controller->CenterOn(my_x, my_y);
    }
  }
}

void Scan() {
  out("SCAN\n");
  if (!expectOK()) {
    return;
  }
  int n;
  controller->SetFog();
  in("%d", &n);
  while (n--) {
    char buffer[10];
    in("%s", buffer);
    int x, y;
    in("%d%d", &x, &y);
    x += my_x;
    y += my_y;
    TypPola t = TypPola::kUnknown;
    char c = buffer[0];
    if (c == 'F') {
      t = TypPola::kEmpty;
    } else if (c == 'S') {
      t = TypPola::kStart;
    } else if (c == 'E') {
      t = TypPola::kEnd;
    } else if (c == 'W') {
      t = TypPola::kWall;
    } else if ('0' <= c and c <= '9') {
      t = TypPola::kEmpty;
    } else {
      debug() << imie(buffer);
      assert(false);
    }
    UstawPole(x, y, t);
  }
  UstawPole(my_x, my_y, TypPola::kMe);
}

int TurnsLeft() {
  out("TURNS_LEFT\n");
  if (expectOK()) {
    int x;
    in("%d", &x);
    return x;
  }
  return -1;
}

void Algorytm() {
  debug() << "Trying to connect...";
  Connect("localhost", 12345);
  debug() << "Connected!";
  if (!login("team24", "5a6b808ca6")) {
    debug() << "Login failed :(";
    return;
  }
  debug() << "Login successful!";
  while (true) {
    cwait();
    debug() << imie(TurnsLeft());
    Scan();
    TryMove();
  }
  Disconnect();
}

int main(int argc, char** argv) {
  Grid::Options options;
  controller = options.controller();
  auto board = std::make_unique<Grid::SquareBoard>();
  //auto board = std::make_unique<Grid::HexBoard>();
  controller->OnKeyPress(OnKeyPress);
  Grid::RunBoard(argc, argv, options, std::move(board), Algorytm);
  return 0;
}
