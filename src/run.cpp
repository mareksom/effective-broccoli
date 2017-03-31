#include "run.h"

#include <gtkmm/application.h>
#include <gtkmm/window.h>

#include "painter.h"
#include "viewer.h"

namespace {

Glib::RefPtr<Gtk::Application> application;

}  // namespace

int RunBoard(int argc, char** argv,
             Options options, std::unique_ptr<Board> board) {
  board->SetOptions(&options);
  if (!application) {
    application = Gtk::Application::create(argc, argv);
  }
  Gtk::Window window;
  Painter painter(&options, board.get(),
                  options.WindowWidthOnStart(), options.WindowHeightOnStart());
  Viewer viewer(&options, &painter);
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
