#include "options.h"

namespace Grid {

Options::Options() {}

Controller* Options::controller() const {
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

double Options::MessageBoxesMargin() const {
  return message_boxes_margin_;
}

void Options::SetMessageBoxesMargin(double margin) {
  message_boxes_margin_ = margin;
}

double Options::MainMessageBoxMaxWidth() const {
  return main_message_box_max_width_;
}

void Options::SetMainMessageBoxMaxWidth(double width) {
  main_message_box_max_width_ = width;
}

double Options::SingleBoxMessageHeight() const {
  return single_box_message_height_;
}

void Options::SetSingleBoxMessageHeight(double height) {
  single_box_message_height_ = height;
}

}  // namespace Grid
