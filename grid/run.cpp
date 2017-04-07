#include "run.h"

#include <condition_variable>
#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <iostream>
#include <mutex>
#include <thread>

#include "painter.h"
#include "viewer.h"

namespace Grid {

namespace {

Glib::RefPtr<Gtk::Application> application;

}  // namespace

int RunBoard(int argc, char** argv,
             const Options& options, std::unique_ptr<Board> board,
             std::function<void()> user_thread) {
  std::mutex mutex;
  bool is_ready = false;
  std::condition_variable cv;
  std::thread(
      [&mutex, &is_ready, &cv, user_thread]() -> void {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [&is_ready]() -> bool { return is_ready; });
        user_thread();
      }).detach();
  argc = 1;
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
  /* Wakes up the user thread. */ {
    std::unique_lock<std::mutex> lock(mutex);
    is_ready = true;
    cv.notify_one();
  }
  application->run(window);
}

}  // namespace Grid
