#include "controller.h"

#include "painter.h"
#include "viewer.h"

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

Controller::Controller()
    : options_(nullptr), viewer_(nullptr), painter_(nullptr),
      main_message_box_(0.0 /* R */, 0.0 /* G */, 0.0 /* B */, 1.0 /* A */,
                        MessageBox::TextAlign::kLeft,
                        MessageBox::TextFlow::kFromBottomToTop,
                        MessageBox::TextWrap::kWrap) {}

void Controller::InvalidateField(int x, int y) {
  if (!IsInitialized()) {
    return;
  }
  painter().InvalidateField(x, y);
}

void Controller::CenterOn(int x, int y) {
  if (!IsInitialized()) {
    return;
  }
  painter().CenterOn(x, y);
  viewer().Redraw();
}

StreamReader Controller::AddMessage() {
  return StreamReader(
      [this](const std::string& message) -> void {
        main_message_box_.AddMessage(message);
        if (IsInitialized()) {
          viewer().Redraw();
        }
      });
}

void Controller::AddSingleMessageBox(
    double r, double g, double b, double a,
    std::function<void(StreamReader&)> generator) {
  single_message_boxes_.emplace_back(r, g, b, a, std::move(generator));
  if (IsInitialized()) {
    viewer().Redraw();
  }
}

void Controller::Draw(double width, double height,
                      const Cairo::RefPtr<Cairo::Context>& context) {
  if (!IsInitialized()) {
    return;
  }
  const double margin = options().MessageBoxesMargin();
  main_message_box_.Draw(
      margin, margin,
      std::min(options().MainMessageBoxMaxWidth(), width / 2) - 2 * margin,
      height - 2 * margin,
      context);
  const double sbm_height = options().SingleBoxMessageHeight();
  double next_sbm_y = margin;
  for (auto& smb : single_message_boxes_) {
    smb.Draw(width - margin, next_sbm_y, sbm_height, context);
    next_sbm_y += sbm_height + margin / 2;
  }
}

bool Controller::IsInitialized() const {
  return options_ != nullptr and viewer_ != nullptr and painter_ != nullptr;
}

void Controller::SetOptions(const Options* options) {
  options_ = options;
}

void Controller::SetViewer(Viewer* viewer) {
  viewer_ = viewer;
}

void Controller::SetPainter(Painter* painter) {
  painter_ = painter;
}

const Options& Controller::options() {
  return *options_;
}

Viewer& Controller::viewer() {
  return *viewer_;
}

Painter& Controller::painter() {
  return *painter_;
}
