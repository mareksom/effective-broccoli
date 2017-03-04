#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <thread>

#include "makra.h"
#include "options.h"
#include "painter.h"
#include "square_board.h"
#include "viewer.h"

int main(int argc, char** argv) {
  Glib::RefPtr<Gtk::Application> application =
      Gtk::Application::create(argc, argv);
  Gtk::Window window;

  auto board = std::make_unique<SquareBoard>();

  Painter painter(
    board.get(), options.WindowWidthOnStart(), options.WindowHeightOnStart());
  Viewer viewer(&painter);
  painter.SetViewer(&viewer);
  painter.Start();

  window.add(viewer);
  window.resize(options.WindowWidthOnStart(), options.WindowHeightOnStart());
  if (options.MaximizeOnStart()) {
    window.maximize();
  }

  viewer.show();

  return application->run(window);
}
