#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <string>

int MakeColor(int r, int g, int b);

double GetDoubleR(int color);
double GetDoubleG(int color);
double GetDoubleB(int color);

class Controller {
 public:
  virtual ~Controller() = default;

  // You -> Board.
  // -------------

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
};

#endif  // CONTROLLER_H_
