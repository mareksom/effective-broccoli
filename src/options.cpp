#include "options.h"

Options::Options() {}

Controller* Options::controller() {
  return controller_;
}

void Options::SetController(Controller* controller) {
  controller_ = controller;
}

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

int Options::NumberOfFieldsProcessedPerFrame() const {
  return number_of_fields_processed_per_frame_;
}

void Options::SetNumberOfFieldsProcessedPerFrame(int number_of_fields) {
  number_of_fields_processed_per_frame_ = number_of_fields;
}

double Options::InitialScale() const {
  return initial_scale_;
}

void Options::SetInitialScale(double scale) {
  initial_scale_ = scale;
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

int Options::NullColor() const {
  return null_color_;
}

void Options::SetNullColor(int color) {
  null_color_ = color;
}

Options options;
