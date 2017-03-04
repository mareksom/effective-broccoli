#ifndef VIEWER_H_
#define VIEWER_H_

#include <cairomm/context.h>
#include <cairomm/refptr.h>
#include <cairomm/surface.h>
#include <glibmm/dispatcher.h>
#include <gtkmm/drawingarea.h>

class Painter;

class Viewer : public Gtk::DrawingArea {
 public:
  Viewer(Painter* painter);

  void Redraw();

  bool on_draw(const Cairo::RefPtr<Cairo::Context>& context) override;
  bool on_scroll_event(GdkEventScroll* event) override;

 private:
  Painter* painter_;

  Glib::Dispatcher redraw_signal_;
};

#endif  // VIEWER_H_
