#include "options.h"

Options::Options() {}

bool Options::MaximizeOnStart() const {
  return maximize_on_start_;
}

void Options::SetMaximizeOnStart(bool maximize) {
  maximize_on_start_ = maximize;
}

int Options::WindowWidthOnStart() const {
  return window_width_on_start_;
}

int Options::WindowHeightOnStart() const {
  return window_height_on_start_;
}

void Options::SetWindowSizeOnStart(int width, int height) {
  window_width_on_start_ = width;
  window_height_on_start_ = height;
}

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
