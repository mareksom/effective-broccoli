#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <memory>
#include <string>

#include "options.h"

class Board;
class Painter;

int MakeColor(int r, int g, int b);

double GetDoubleR(int color);
double GetDoubleG(int color);
double GetDoubleB(int color);

class Controller {
 public:
  Controller();
  virtual ~Controller() = default;

  // You -> Board.
  // -------------

  void InvalidateField(int x, int y);
  void CenterOn(int x, int y);


  // Board -> You -> Board.
  // ----------------------

  virtual int BoardWidth() = 0;
  virtual int BoardHeight() = 0;

  virtual int GetFieldColor(int x, int y) = 0;
  virtual int GetObject(int x, int y) = 0;


  // Board -> You.
  // -------------

  // A button was clicked:
  //  1 - left
  //  2 - middle
  //  3 - right
  // If there are more buttons, it is possible that @button will not belong
  // to the set {1, 2, 3}.
  virtual void FieldClick(int x, int y, int button) = 0;

  virtual void KeyPress(const std::string& key) = 0;

 private:
  friend int RunBoard(int argc, char** argv,
                      Options options, std::unique_ptr<Board> board);
  void SetPainter(Painter* painter);

  Painter& painter();

  Painter* painter_;
};

#endif  // CONTROLLER_H_
