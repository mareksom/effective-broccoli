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

  double x_, y_;
  double scale_;

  Glib::Dispatcher dispatcher_;
};

#endif  // VIEWER_H_
