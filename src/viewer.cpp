#include "viewer.h"

#include "makra.h"
#include "painter.h"

Viewer::Viewer(Painter* painter) : painter_(painter) {
  add_events(Gdk::SCROLL_MASK);
  add_events(Gdk::KEY_PRESS_MASK);
  add_events(Gdk::KEY_RELEASE_MASK);
  set_can_focus(true);
  grab_focus();
  dispatcher_.connect(
      [this]() -> void {
        queue_draw();
      });
  x_ = 0;
  y_ = 0;
  scale_ = 100;
  painter_->SetTransformation(x_, y_, scale_);
}

void Viewer::Redraw() {
  dispatcher_.emit();
}

bool Viewer::on_draw(const Cairo::RefPtr<Cairo::Context>& context) {
  auto surface = painter_->GetAndLockCurrentSurface();
  const double widget_width = get_allocated_width();
  const double widget_height = get_allocated_height();
  context->save();
    context->set_source(*surface, -widget_width / 4, -widget_height / 4);
    context->paint();
  context->restore();
  painter_->ReleaseCurrentSurface();
  return true;
}

bool Viewer::on_scroll_event(GdkEventScroll* event) {
  if (event->state & GDK_CONTROL_MASK) {
    double factor = 1;
    switch (event->direction) {
      case GDK_SCROLL_UP:
        factor = 1.2;
        break;
      case GDK_SCROLL_DOWN:
        factor = 1 / 1.2;
        break;
    }
    scale_ *= factor;
  } else {
    int delta_x = 0;
    int delta_y = 0;
    switch (event->direction) {
      case GdkScrollDirection::GDK_SCROLL_UP:
        delta_y--;
        break;
      case GdkScrollDirection::GDK_SCROLL_DOWN:
        delta_y++;
        break;
      case GdkScrollDirection::GDK_SCROLL_LEFT:
        delta_x--;
        break;
      case GdkScrollDirection::GDK_SCROLL_RIGHT:
        delta_x++;
        break;
    }
    x_ += delta_x * 20 / scale_;
    y_ += delta_y * 20 / scale_;
  }
  debug() << imie(x_) << imie(y_) << imie(scale_);
  painter_->SetTransformation(x_, y_, scale_);
  return true;
}
