#include "controller.h"

#include <limits>

#include "options.h"
#include "painter.h"
#include "viewer.h"

namespace Grid {

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
                        MessageBox::TextWrap::kWrap),
      on_field_click_callback_([](int, int, int) -> void {}),
      on_key_press_callback_([](const std::string&) -> void {}),
      current_time_(std::numeric_limits<int64_t>::min()),
      min_x_(0), max_x_(0), min_y_(0), max_y_(0),
      fields_() {}

void Controller::SetFieldColor(int x, int y, int r, int g, int b) {
  /* Lock */ {
    std::lock_guard<std::mutex> lock(mutex_);
    Field* field = GetField(x, y, true /* force */);
    field->background = MakeColor(r, g, b);
    field->last_update_time = current_time_;
  }
  InvalidateField(x, y);
}

void Controller::SetObject(int x, int y,
                           Object object, int r, int g, int b) {
  /* Lock */ {
    std::lock_guard<std::mutex> lock(mutex_);
    Field* field = GetField(x, y, true /* force */);
    field->object = MakeObject(object, r, g, b);
    field->last_update_time = current_time_;
  }
  InvalidateField(x, y);
}

StreamReader Controller::SetText(int x, int y) {
  return StreamReader(
      [this, x, y](const std::string& message) -> void {
        /* Lock */ {
          std::lock_guard<std::mutex> lock(mutex_);
          Field* field = GetField(x, y, true /* force */);
          field->text = message;
          field->last_update_time = current_time_;
        }
        InvalidateField(x, y);
      });
}

void Controller::SetFog() {
  /* Lock */ {
    std::lock_guard<std::mutex> lock(mutex_);
    current_time_++;
  }
  InvalidateEverything();
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
        std::lock_guard<std::mutex> lock(mutex_);
        main_message_box_.AddMessage(message);
        if (IsInitialized()) {
          viewer().Redraw();
        }
      });
}

void Controller::AddSingleMessageBox(
    double r, double g, double b, double a,
    std::function<void(StreamReader&)> generator) {
  std::lock_guard<std::mutex> lock(mutex_);
  single_message_boxes_.emplace_back(r, g, b, a, std::move(generator));
  if (IsInitialized()) {
    viewer().Redraw();
  }
}

void Controller::OnFieldClick(std::function<void(int, int, int)> callback) {
  std::lock_guard<std::mutex> lock(mutex_);
  on_field_click_callback_ = callback;
}

void Controller::OnKeyPress(std::function<void(const std::string&)> callback) {
  std::lock_guard<std::mutex> lock(mutex_);
  on_key_press_callback_ = callback;
}

void Controller::GetExtensions(int& min_x, int& min_y, int& max_x, int& max_y) {
  std::lock_guard<std::mutex> lock(mutex_);
  min_x = min_x_;
  min_y = min_y_;
  max_x = max_x_;
  max_y = max_y_;
}

void Controller::GetFieldInfo(int x, int y, bool& border, int& background,
                              int& object, std::string& text, bool& fog) {
  std::lock_guard<std::mutex> lock(mutex_);
  Field* field = GetField(x, y, false /* don't force */);
  if (field == nullptr) {
    const int null_color = options().NullColor();
    border = false;
    background = MakeColor(null_color, null_color, null_color);
    object = MakeObject(Object::kNone, 0, 0, 0);
    text.clear();
    fog = false;
    return;
  }
  border = true;
  background = field->background;
  object = field->object;
  text = field->text;
  fog = (field->last_update_time < current_time_);
}

void Controller::FieldClick(int x, int y, int button) {
  std::function<void(int, int, int)> copy;
  /* Lock */ {
    std::lock_guard<std::mutex> lock(mutex_);
    copy = on_field_click_callback_;
  }
  copy(x, y, button);
}

void Controller::KeyPress(const std::string& key) {
  std::function<void(const std::string&)> copy;
  /* Lock */ {
    std::lock_guard<std::mutex> lock(mutex_);
    copy = on_key_press_callback_;
  }
  copy(key);
}

void Controller::Draw(double width, double height,
                      const Cairo::RefPtr<Cairo::Context>& context) {
  if (!IsInitialized()) {
    return;
  }
  std::lock_guard<std::mutex> lock(mutex_);
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

void Controller::InvalidateField(int x, int y) {
  if (!IsInitialized()) {
    return;
  }
  painter().InvalidateField(x, y);
}

void Controller::InvalidateEverything() {
  if (!IsInitialized()) {
    return;
  }
  painter().InvalidateEverything();
}

Controller::Field* Controller::GetField(int x, int y, bool force) {
  auto it = fields_.find(std::make_pair(x, y));
  if (it == fields_.end()) {
    if (!force) {
      return nullptr;
    }
    const int null_color = options().NullColor();
    Field default_field;
    default_field.background = MakeColor(null_color, null_color, null_color);
    default_field.object = MakeObject(Object::kNone, 0, 0, 0);
    default_field.last_update_time = current_time_;
    it = fields_.emplace(std::make_pair(x, y), default_field).first;
    if (x < min_x_) {
      min_x_ = x;
    }
    if (max_x_ < x) {
      max_x_ = x;
    }
    if (y < min_y_) {
      min_y_ = y;
    }
    if (max_y_ < y) {
      max_y_ = y;
    }
  }
  return &(it->second);
}

}  // namespace Grid
