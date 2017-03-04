#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <thread>

#include "makra.h"
#include "painter.h"
#include "square_board.h"
#include "viewer.h"

int main(int argc, char** argv) {
  Glib::RefPtr<Gtk::Application> application =
      Gtk::Application::create(argc, argv);
  Gtk::Window window;

  auto board = std::make_unique<SquareBoard>();

  #warning Change resolution.
  Painter painter(board.get(), 600, 600);
  Viewer viewer(&painter);
  painter.SetViewer(&viewer);
  painter.Start();

  window.add(viewer);
  window.resize(600, 600);
  //window.maximize();

  viewer.show();

  return application->run(window);
}
