#include "options.h"

Options::Options() {}

double Options::ScrollSpeed() const {
  return scroll_speed_;
}

void Options::SetScrollSpeed(double speed) {
  scroll_speed_ = speed;
}

double Options::ZoomSpeed() const {
  return zoom_speed_;
}

void Options::SetZoomSpeed(double speed) {
  zoom_speed_ = speed;
}

Options options;
