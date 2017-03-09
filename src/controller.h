#ifndef CONTROLLER_H_
#define CONTROLLER_H_

int MakeColor(int r, int g, int b);

double GetDoubleR(int color);
double GetDoubleG(int color);
double GetDoubleB(int color);

class Controller {
 public:
  virtual int BoardWidth() = 0;
  virtual int BoardHeight() = 0;

  virtual int GetFieldColor(int x, int y) = 0;
  virtual int GetObject(int x, int y) = 0;
};

#endif  // CONTROLLER_H_
