#ifndef GRID_VIEWER_H_
#define GRID_VIEWER_H_

#include <cairomm/context.h>
#include <cairomm/refptr.h>
#include <cairomm/surface.h>
#include <glibmm/dispatcher.h>
#include <gtkmm/drawingarea.h>
#include <utility>

namespace Grid {

class Options;
class Painter;

class Viewer : public Gtk::DrawingArea {
 public:
  Viewer(const Options* options, Painter* painter);

  void Redraw();

 protected:
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& context) override;
  bool on_scroll_event(GdkEventScroll* event) override;
  bool on_configure_event(GdkEventConfigure* event) override;

  bool on_button_press_event(GdkEventButton* event) override;
  bool on_button_release_event(GdkEventButton* event) override;

  bool on_key_press_event(GdkEventKey* event) override;

 private:
  const Options& options() const;

  const Options* options_;
  Painter* painter_;

  unsigned press_button_;
  std::pair<int, int> press_point_;

  Glib::Dispatcher redraw_signal_;
};

}  // namespace Grid

#endif  // GRID_VIEWER_H_
