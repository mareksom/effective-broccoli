#include <boost/gil/extension/io/png_dynamic_io.hpp>
#include <boost/gil/gil_all.hpp>

namespace gil = boost::gil;

#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <thread>

#include "controller.h"
#include "hex_board.h"
#include "makra.h"
#include "object.h"
#include "options.h"
#include "painter.h"
#include "square_board.h"
#include "viewer.h"

struct Contr : Controller {
  gil::rgb8_image_t input;

  void Init() {
    gil::png_read_and_convert_image("obrazek.png", input);
    debug() << imie(BoardWidth()) << imie(BoardHeight());
  }

  int BoardWidth() override {
    return input.width() * 4;
  }

  int BoardHeight() override {
    return input.height();
  }

  int GetFieldColor(int x, int y) override {
    gil::rgb8_pixel_t pixel = view(input)(x, y);
    return MakeColor(pixel[0], pixel[1], pixel[2]);
  }

  int GetObject(int x, int y) override {
    srand(50 * 1000 * x + y);
    const int wybor = rand() % static_cast<int>(Object::kCount);
    const int r = rand() % 256;
    const int g = rand() % 256;
    const int b = rand() % 256;
    return MakeObject(static_cast<Object>(wybor), r, g, b);
  }
} controller;

int main(int argc, char** argv) {
  controller.Init();

  options.SetController(&controller);

  Glib::RefPtr<Gtk::Application> application =
      Gtk::Application::create(argc, argv);
  Gtk::Window window;

  std::unique_ptr<Board> board;
  char buf[10];
  scanf("%s", buf);
  if (buf[0] == 's') {
    board = std::make_unique<SquareBoard>();
  } else {
    board = std::make_unique<HexBoard>();
  }

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
