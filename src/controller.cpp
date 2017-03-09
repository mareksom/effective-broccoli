#include "controller.h"

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
