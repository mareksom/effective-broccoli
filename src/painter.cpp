#include "painter.h"

#include <cassert>
#include <chrono>

#include "board.h"
#include "makra.h"
#include "surface_utils.h"
#include "viewer.h"

#warning Zmienić sklaę 100 na coś sensowngeo (np. 1).
Painter::Painter(Board* board, int width, int height)
    : board_(board), viewer_(nullptr),
      modification_{width / 2.0, height / 2.0, 100, width, height},
      // @width_ and @height_ will be initialized after first modification.
      width_(0), height_(0),
      tx_(0), ty_(0), micro_dx_(0), micro_dy_(0), scale_(100) {
  assert(width > 0);
  assert(height > 0);
  // Sets up main surfaces.
  for (int i = 0; i < 2; i++) {
    main_surface_[i] = Cairo::ImageSurface::create(
        Cairo::Format::FORMAT_RGB24, width * 2, height * 2);
  }
  current_main_surface_ = 0;
  context_ = Cairo::Context::create(main_surface_[current_main_surface_]);
  // Sets up surface buffers.
  for (int i = 0; i < 3; i++) {
    surface_buffers_[i].surface = Cairo::ImageSurface::create(
        Cairo::Format::FORMAT_RGB24, width * 2, height * 2);
    surface_buffers_[i].start_x = 0;
    surface_buffers_[i].start_y = 0;
    surface_buffer_updater_.AddObject(&surface_buffers_[i]);
  }
  surface_buffer_updater_.SetCurrentObject(
      surface_buffer_updater_.GetFreeObject());
  // The initializing modification.
  SetModification();
}

void Painter::SetViewer(Viewer* viewer) {
  assert(viewer_ == nullptr);
  viewer_ = viewer;
}

void Painter::Start() {
  std::thread([this]() -> void {
                DrawLoop();
              }).detach();
}

const Painter::SurfaceBuffer* Painter::GetAndLockCurrentSurfaceBuffer() {
  return surface_buffer_updater_.LockCurrentObject();
}

void Painter::ReleaseCurrentSurfaceBuffer() {
  surface_buffer_updater_.ReleaseCurrentObject();
}

void Painter::Resize(int width, int height) {
  modification_.tx -= (modification_.width - width) / 2.0;
  modification_.ty -= (modification_.height - height) / 2.0;
  modification_.width = width;
  modification_.height = height;
  SetModification();
}

void Painter::Translate(double dx, double dy) {
  modification_.tx += dx;
  modification_.ty += dy;
  SetModification();
}

void Painter::Zoom(double x, double y, double factor) {
  x += modification_.width / 2.0;
  y += modification_.height / 2.0;
  modification_.tx = x + (modification_.tx - x) * factor;
  modification_.ty = y + (modification_.ty - y) * factor;
  modification_.scale *= factor;
  SetModification();
}

std::pair<double, double> Painter::BoardToSurfaceCoordinates(
    double x, double y) const {
  return std::make_pair(x * scale_ + tx_, y * scale_ + ty_);
}

std::pair<double, double> Painter::SurfaceToBoardCoordinates(
    double x, double y) const {
  return std::make_pair((x - tx_) / scale_, (y - ty_) / scale_);
}

void Painter::SetModification() {
  Modification* modification = modification_updater_.GetFreeObject();
  *modification = modification_;
  modification_updater_.SetCurrentObject(modification);
}

void Painter::UpdateCurrentSurface() {
  SurfaceBuffer* surface_buffer = surface_buffer_updater_.GetFreeObject();
  if (surface_buffer->surface->get_width() != width_ * 2 or
      surface_buffer->surface->get_height() != height_ * 2) {
    surface_buffer->surface = Cairo::ImageSurface::create(
        Cairo::Format::FORMAT_RGB24, width_ * 2, height_ * 2);
  }
  CopySurface(main_surface_[current_main_surface_], surface_buffer->surface);
  surface_buffer->start_x = -width_ / 2.0 + micro_dx_;
  surface_buffer->start_y = -height_ / 2.0 + micro_dy_;
  surface_buffer_updater_.SetCurrentObject(surface_buffer);
  viewer_->Redraw();
}

void Painter::DrawLoop() {
  while (true) {
    const Modification* mod =
        modification_updater_.LockCurrentObject(false /* Don't block. */);
    if (mod == nullptr and fields_to_draw_.empty()) {
      mod = modification_updater_.LockCurrentObject(true /* Do block. */);
    }
    if (mod != nullptr) {
      ApplyModification(mod);
      modification_updater_.ReleaseCurrentObject();
    } else {
      ProcessSomeFields();
    }
  }
}

void Painter::ApplyTranslation(int dx, int dy) {
  auto old_ul = SurfaceToBoardCoordinates(0, 0);
  auto old_lr = SurfaceToBoardCoordinates(width_ * 2, height_ * 2);
  tx_ += dx;
  ty_ += dy;
  ShiftSurface(main_surface_[current_main_surface_], dx, dy);
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

  if (dx == 0 and dy == 0) {
    // Nothing to do.
  } else if (dx >= 0 and dy >= 0) {
    ClearRectangle(lr.first, old_ul.second, old_lr.first, old_lr.second);
    ClearRectangle(old_ul.first, lr.second, lr.first, old_lr.second);
    AddRectangle(ul.first, ul.second, old_ul.first, lr.second);
    AddRectangle(old_ul.first, ul.second, lr.first, old_ul.second);
  } else if (dx < 0 and dy < 0) {
    ClearRectangle(old_ul.first, old_ul.second, ul.first, old_lr.second);
    ClearRectangle(ul.first, old_ul.second, old_lr.first, ul.second);
    AddRectangle(old_lr.first, ul.second, lr.first, lr.second);
    AddRectangle(ul.first, old_lr.second, old_lr.first, lr.second);
  } else if (dx < 0) {
    assert(dy >= 0);
    ClearRectangle(old_ul.first, old_ul.second, ul.first, old_lr.second);
    ClearRectangle(ul.first, lr.second, old_lr.first, old_lr.second);
    AddRectangle(old_lr.first, ul.second, lr.first, lr.second);
    AddRectangle(ul.first, ul.second, old_lr.first, old_ul.second);
  } else {
    assert(dx >= 0);
    assert(dy < 0);
    ClearRectangle(lr.first, old_ul.second, old_lr.first, old_lr.second);
    ClearRectangle(old_ul.first, old_ul.second, old_lr.first, ul.second);
    AddRectangle(ul.first, ul.second, old_ul.first, lr.second);
    AddRectangle(old_ul.first, old_lr.second, lr.first, lr.second);
  }
  UpdateCurrentSurface();
}

void Painter::ApplyZoom(int new_tx, int new_ty, double new_scale) {
  const double scale_diff = new_scale - scale_;
  const double fix_x = (tx_ * new_scale - new_tx * scale_) / scale_diff;
  const double fix_y = (ty_ * new_scale - new_ty * scale_) / scale_diff;
  current_main_surface_ ^= 1;
  context_ = Cairo::Context::create(main_surface_[current_main_surface_]);
  context_->save();
    context_->set_source_rgb(0, 0, 0);
    context_->paint();
    context_->translate(fix_x, fix_y);
    context_->scale(new_scale / scale_, new_scale / scale_);
    context_->set_source(
        main_surface_[current_main_surface_ ^ 1], -fix_x, -fix_y);
    context_->paint();
  context_->restore();
  tx_ = new_tx;
  ty_ = new_ty;
  scale_ = new_scale;
  auto upper_left = SurfaceToBoardCoordinates(0, 0);
  auto lower_right = SurfaceToBoardCoordinates(width_ * 2, height_ * 2);
  fields_to_draw_.clear();
  board_->IterateFieldsInRectangle(
      upper_left.first, upper_left.second,
      lower_right.first, lower_right.second,
      [this](int x, int y) -> void {
        fields_to_draw_.emplace(x, y);
      });
  UpdateCurrentSurface();
}

void Painter::ApplyBruteForceModification(int tx, int ty, double scale) {
  tx_ = tx;
  ty_ = ty;
  scale_ = scale;
  auto upper_left = SurfaceToBoardCoordinates(0, 0);
  auto lower_right = SurfaceToBoardCoordinates(width_ * 2, height_ * 2);
  fields_to_draw_.clear();
  board_->IterateFieldsInRectangle(
      upper_left.first, upper_left.second,
      lower_right.first, lower_right.second,
      [this](int x, int y) -> void {
        fields_to_draw_.emplace(x, y);
      });
  context_->save();
    context_->set_source_rgb(0, 0, 0);
    context_->paint();
  context_->restore();
  UpdateCurrentSurface();
}

void Painter::ApplyModification(const Modification* modification) {
  const int new_tx = static_cast<int>(modification->tx);
  const int new_ty = static_cast<int>(modification->ty);
  const double new_scale = modification->scale;
  micro_dx_ = modification->tx - new_tx;
  micro_dy_ = modification->ty - new_ty;
  if (modification->width != width_ or modification->height != height_) {
    // Resizes only main surfaces.  Surface buffers will be lazily updated.
    width_ = modification->width;
    height_ = modification->height;
    context_.clear();
    for (int i = 0; i < 2; i++) {
      main_surface_[i] = Cairo::ImageSurface::create(
          Cairo::Format::FORMAT_RGB24, width_ * 2, height_ * 2);
    }
    context_ = Cairo::Context::create(main_surface_[current_main_surface_]);
    return ApplyBruteForceModification(new_tx, new_ty, new_scale);
  }
  if (std::abs(scale_ - new_scale) < 1e-9 and
      std::abs(new_tx - tx_) <= width_ * 3 / 2 and
      std::abs(new_ty - ty_) <= height_ * 3 / 2) {
    return ApplyTranslation(new_tx - tx_, new_ty - ty_);
  }
  if (std::abs(scale_ - new_scale) > 1e-9) {
    return ApplyZoom(new_tx, new_ty, new_scale);
  }
  ApplyBruteForceModification(new_tx, new_ty, new_scale);
}

void Painter::ProcessSomeFields() {
  if (fields_to_draw_.empty()) return;
  int cnt = 500;
  while (!fields_to_draw_.empty() and cnt-- > 0) {
    auto it = fields_to_draw_.begin();
    const int x = it->first;
    const int y = it->second;
    fields_to_draw_.erase(it);
    context_->save();
      context_->translate(tx_, ty_);
      context_->scale(scale_, scale_);
      board_->DrawField(x, y, context_);
    context_->restore();
  }
  UpdateCurrentSurface();
}
