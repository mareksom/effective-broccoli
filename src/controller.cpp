#include "controller.h"

#include "painter.h"

int MakeColor(int r, int g, int b) {
  return (r << 16) | (g << 8) | b;
}

double GetDoubleR(int color) {
  return ((color >> 16) & 255) / 255.0;
}

double GetDoubleG(int color) {
  return ((color >> 8) & 255) / 255.0;
}
double GetDoubleB(int color) {
  return (color & 255) / 255.0;
}

Controller::Controller() : painter_(nullptr) {}

void Controller::InvalidateField(int x, int y) {
  painter().InvalidateField(x, y);
}

void Controller::CenterOn(int x, int y) {
  painter().CenterOn(x, y);
}

void Controller::SetPainter(Painter* painter) {
  painter_ = painter;
}

Painter& Controller::painter() {
  return *painter_;
}
