#include "viewer.h"

#include <cmath>

#include "makra.h"
#include "options.h"
#include "painter.h"

Viewer::Viewer(Painter* painter) : painter_(painter) {
  add_events(Gdk::SMOOTH_SCROLL_MASK);
  add_events(Gdk::KEY_PRESS_MASK);
  add_events(Gdk::KEY_RELEASE_MASK);
  set_can_focus(true);
  grab_focus();
  redraw_signal_.connect(
      [this]() -> void {
        queue_draw();
      });
}

void Viewer::Redraw() {
  redraw_signal_.emit();
}

bool Viewer::on_draw(const Cairo::RefPtr<Cairo::Context>& context) {
  const Painter::SurfaceBuffer* surface_buffer =
      painter_->GetAndLockCurrentSurfaceBuffer();
  context->save();
    context->set_source(surface_buffer->surface,
                        surface_buffer->start_x, surface_buffer->start_y);
    debug() << imie(surface_buffer->start_x) << imie(surface_buffer->start_y);
    context->paint();
  context->restore();
  painter_->ReleaseCurrentSurfaceBuffer();
  return true;
}

bool Viewer::on_scroll_event(GdkEventScroll* event) {
  if (event->state & GDK_CONTROL_MASK) {
    if (event->direction == GDK_SCROLL_SMOOTH) {
      painter_->Zoom(event->x, event->y,
                     std::pow(options.ZoomSpeed(), -event->delta_y));
    }
  } else {
    if (event->direction == GDK_SCROLL_SMOOTH) {
      painter_->Translate(-event->delta_x * options.ScrollSpeed(),
                          -event->delta_y * options.ScrollSpeed());
    }
  }
  return true;
}
