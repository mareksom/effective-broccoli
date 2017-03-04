#include "painter.h"

#include <cassert>
#include <chrono>

#include "board.h"
#include "makra.h"
#include "surface_utils.h"
#include "viewer.h"

Painter::Painter(Board* board, int width, int height)
    : board_(board), viewer_(nullptr),
      width_(width), height_(height), x_(0), y_(0), scale_(1) {
  assert(width > 0);
  assert(height > 0);
  main_surface_ = Cairo::ImageSurface::create(
      Cairo::Format::FORMAT_RGB24, width * 2, height * 2);
  context_ = Cairo::Context::create(main_surface_);
  for (int i = 0; i < 3; i++) {
    surfaces[i] = Cairo::ImageSurface::create(
        Cairo::Format::FORMAT_RGB24, width * 2, height * 2);
    object_updater_.AddObject(&surfaces[i]);
  }
  object_updater_.SetCurrentObject(object_updater_.GetFreeObject());
}

void Painter::SetViewer(Viewer* viewer) {
  assert(viewer_ == nullptr);
  viewer_ = viewer;
}

void Painter::Start() {
  #warning Zanim wywoła się pierwszy obrót pętli, to current_surface_ jest nullem.
  thread_ = std::thread(
      [this]() -> void {
        DrawLoop();
      });
  thread_.detach();
}

const Cairo::RefPtr<Cairo::ImageSurface>* Painter::GetAndLockCurrentSurface() {
  return object_updater_.LockCurrentObject();
}

void Painter::ReleaseCurrentSurface() {
  object_updater_.ReleaseCurrentObject();
}

void Painter::SetTransformation(double x, double y, double scale) {
  debug() << "Painter::SetTransformation(" << imie(x) << imie(y) << imie(scale) << ")";
  Modification* mod = object_updater_consume_.GetFreeObject();
  mod->x = x;
  mod->y = y;
  mod->scale = scale;
  object_updater_consume_.SetCurrentObject(mod);
}

std::pair<double, double> Painter::BoardToSurfaceCoordinates(
    double x, double y) const {
  return std::make_pair((x - x_) * scale_ + width_,
                        (y - y_) * scale_ + height_);
}

std::pair<double, double> Painter::SurfaceToBoardCoordinates(
    double x, double y) const {
  return std::make_pair((x - width_) / scale_ + x_,
                        (y - height_) / scale_ + y_);
}

void Painter::UpdateCurrentSurface() {
  auto surface = object_updater_.GetFreeObject();
  CopySurface(main_surface_, *surface);
  object_updater_.SetCurrentObject(surface);
  viewer_->Redraw();
}

void Painter::DrawLoop() {
  while (true) {
    const Modification* mod =
        object_updater_consume_.LockCurrentObject(false /* Don't block. */);
    if (mod == nullptr and fields_to_draw_.empty()) {
      mod = object_updater_consume_.LockCurrentObject(true /* Do block. */);
    }
    if (mod != nullptr) {
      ApplyModification(mod);
      object_updater_consume_.ReleaseCurrentObject();
    } else {
      ProcessAField();
    }
  }
}

void Painter::ApplyTranslation(double dx, double dy) {
  #warning Zrobić smooth przesuwanie, przekładając obowiązek mikroprzesunięcia na Widget.
  int dix = static_cast<int>(std::round(dx * scale_));
  int diy = static_cast<int>(std::round(dy * scale_));
  if (dix == 0 and diy == 0) {
    return;
  }
  auto old_ul = SurfaceToBoardCoordinates(0, 0);
  auto old_lr = SurfaceToBoardCoordinates(width_ * 2, height_ * 2);
  x_ += dix / scale_;
  y_ += diy / scale_;
  ShiftSurface(main_surface_, -dix, -diy);
  auto ul = SurfaceToBoardCoordinates(0, 0);
  auto lr = SurfaceToBoardCoordinates(width_ * 2, height_ * 2);

  auto ClearRectangle = [this](
      double left, double top, double right, double bottom) -> void {
    board_->IterateFieldsInRectangle(
        left, top, right, bottom,
        [this](int x, int y) -> void {
          fields_to_draw_.erase(std::make_pair(x, y));
        });
  };

  auto AddRectangle = [this](
      double left, double top, double right, double bottom) -> void {
    board_->IterateFieldsInRectangle(
        left, top, right, bottom,
        [this](int x, int y) -> void {
          fields_to_draw_.emplace(x, y);
        });
  };

  if (dix <= 0 and diy <= 0) {
    ClearRectangle(lr.first, old_ul.second, old_lr.first, old_lr.second);
    ClearRectangle(old_ul.first, lr.second, lr.first, old_lr.second);
    AddRectangle(ul.first, ul.second, old_ul.first, lr.second);
    AddRectangle(old_ul.first, ul.second, lr.first, old_ul.second);
  } else if (dix > 0 and diy > 0) {
    ClearRectangle(old_ul.first, old_ul.second, ul.first, old_lr.second);
    ClearRectangle(ul.first, old_ul.second, old_lr.first, ul.second);
    AddRectangle(old_lr.first, ul.second, lr.first, lr.second);
    AddRectangle(ul.first, old_lr.second, old_lr.first, lr.second);
  } else if (dix > 0) {
    assert(diy <= 0);
    ClearRectangle(old_ul.first, old_ul.second, ul.first, old_lr.second);
    ClearRectangle(ul.first, lr.second, old_lr.first, old_lr.second);
    AddRectangle(old_lr.first, ul.second, lr.first, lr.second);
    AddRectangle(ul.first, ul.second, old_lr.first, old_ul.second);
  } else {
    assert(dix <= 0);
    assert(diy > 0);
    ClearRectangle(lr.first, old_ul.second, old_lr.first, old_lr.second);
    ClearRectangle(old_ul.first, old_ul.second, old_lr.first, ul.second);
    AddRectangle(ul.first, ul.second, old_ul.first, lr.second);
    AddRectangle(old_ul.first, old_lr.second, lr.first, lr.second);
  }
  UpdateCurrentSurface();
}

void Painter::ApplyModification(const Modification* modification) {
  if (std::abs(scale_ - modification->scale) < 1e-9) {
    ApplyTranslation(modification->x - x_, modification->y - y_);
  } else {
    x_ = modification->x;
    y_ = modification->y;
    scale_ = modification->scale;
    fields_to_draw_.clear();
    auto upper_left = SurfaceToBoardCoordinates(0, 0);
    auto lower_right = SurfaceToBoardCoordinates(width_ * 2, height_ * 2);
    board_->IterateFieldsInRectangle(
        upper_left.first, upper_left.second,
        lower_right.first, lower_right.second,
        [this](int x, int y) -> void {
          fields_to_draw_.emplace(x, y);
        });
  }
}

void Painter::ProcessAField() {
  if (fields_to_draw_.empty()) return;
  int cnt = 200;
  while (!fields_to_draw_.empty() and cnt-- > 0) {
    auto it = fields_to_draw_.begin();
    const int x = it->first;
    const int y = it->second;
    fields_to_draw_.erase(it);
    context_->save();
      context_->translate(width_, height_);
      context_->scale(scale_, scale_);
      context_->translate(-x_, -y_);
      board_->DrawField(x, y, context_);
    context_->restore();
  }
  UpdateCurrentSurface();
}
