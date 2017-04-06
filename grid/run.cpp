#include "run.h"

#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <iostream>

#include "painter.h"
#include "viewer.h"

namespace Grid {

namespace {

Glib::RefPtr<Gtk::Application> application;

}  // namespace

int RunBoard(int argc, char** argv,
             Options options, std::unique_ptr<Board> board) {
  argc = 1;
  if (options.controller() == nullptr) {
    std::cerr << "You must set a controller: "
                 "options.SetController(my_controller);" << std::endl;
    return -1;
  }
  board->SetOptions(&options);
  if (!application) {
    application = Gtk::Application::create(argc, argv);
  }
  Gtk::Window window;
  Painter painter(&options, board.get(),
                  options.WindowWidthOnStart(), options.WindowHeightOnStart());
  Viewer viewer(&options, &painter);
  options.controller()->SetOptions(&options);
  options.controller()->SetViewer(&viewer);
  options.controller()->SetPainter(&painter);
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

}  // namespace Grid
