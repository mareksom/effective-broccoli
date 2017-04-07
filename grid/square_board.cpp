#include "square_board.h"

#include <algorithm>
#include <cassert>
#include <string>

#include "controller.h"
#include "makra.h"
#include "object.h"
#include "options.h"

namespace Grid {

namespace {

constexpr double kSqrt2 = 1.4142135623730951;

}  // namespace

std::pair<int, int> SquareBoard::PointToCoordinates(double x, double y) const {
  return std::make_pair(static_cast<int>(std::floor(x)),
                        static_cast<int>(std::floor(y)));
}

std::pair<double, double> SquareBoard::CenterOfField(int x, int y) const {
  return std::make_pair(x + 0.5, y + 0.5);
}

void SquareBoard::IterateFieldsInRectangle(
    double x_min, double y_min, double x_max, double y_max,
    std::function<void(int, int)> callback) const {
  int x_min_int = static_cast<int>(std::floor(x_min));
  int y_min_int = static_cast<int>(std::floor(y_min));
  int x_max_int = static_cast<int>(std::floor(x_max));
  int y_max_int = static_cast<int>(std::floor(y_max));
  int board_x_min, board_y_min, board_x_max, board_y_max;
  options().controller()->GetExtensions(
      board_x_min, board_y_min, board_x_max, board_y_max);
  x_min_int = std::max(x_min_int, board_x_min);
  y_min_int = std::max(y_min_int, board_y_min);
  x_max_int = std::min(x_max_int, board_x_max);
  y_max_int = std::min(y_max_int, board_y_max);
  for (int x = x_min_int; x <= x_max_int; x++) {
    for (int y = y_min_int; y <= y_max_int; y++) {
      callback(x, y);
    }
  }
}

void SquareBoard::DrawField(
    int x, int y, const Cairo::RefPtr<Cairo::Context>& context) const {
  bool border;
  int color, object;
  std::string text;
  bool fog;
  options().controller()->GetFieldInfo(x, y, border, color, object, text, fog);
  context->save();
    context->translate(x + 0.5, y + 0.5);
    // Background.
    context->move_to(-0.5, -0.5);
    context->line_to(0.5, -0.5);
    context->line_to(0.5, 0.5);
    context->line_to(-0.5, 0.5);
    context->close_path();
    context->clip_preserve();
    context->set_source_rgb(
        GetDoubleR(color), GetDoubleG(color), GetDoubleB(color));
    context->fill();
    // Object.
    context->save();
      context->scale(0.4, 0.4);
      DrawObject(context, object);
    context->restore();
    // Text.
    if (!text.empty()) {
      context->save();
        Cairo::TextExtents te;
        context->get_text_extents(text, te);
        const double scale = std::min(0.5 / te.width, 0.5 / te.height);
        context->scale(scale, scale);
        context->get_text_extents(text, te);
        // Background.
        constexpr double border_ratio = 0.05;
        const double width = 0.5 / scale;
        const double border = width * border_ratio;
        context->rectangle(
            width - te.width - 2 * border, width - te.height - 2 * border,
            te.width + 2 * border, te.height + 2 * border);
        context->set_source_rgba(1, 1, 1, 0.6);
        context->fill();
        // Text.
        context->move_to(width - te.width - te.x_bearing - border,
                         width - te.height - te.y_bearing - border);
        context->set_source_rgba(0, 0, 0, 0.6);
        context->show_text(text);
      context->restore();
    }
    if (border) {
      context->move_to(-0.5, -0.5);
      context->line_to(0.5, -0.5);
      context->line_to(0.5, 0.5);
      context->line_to(-0.5, 0.5);
      context->close_path();
      context->set_source_rgb(0, 0, 0);
      context->set_line_width(0.01);
      context->stroke();
    }
    if (fog) {
      auto gradient = Cairo::LinearGradient::create(-0.5, 0.5, 0.5, -0.5);
      gradient->add_color_stop_rgba(0,    0, 0, 0, 0.5);
      gradient->add_color_stop_rgba(0.25, 1, 1, 1, 0);
      gradient->add_color_stop_rgba(0.5,  0, 0, 0, 0.5);
      gradient->add_color_stop_rgba(0.75, 1, 1, 1, 0);
      gradient->add_color_stop_rgba(1,    0, 0, 0, 0.5);
      context->set_source(gradient);
      context->paint();
      gradient = Cairo::LinearGradient::create(0.5, 0.5, -0.5, -0.5);
      gradient->add_color_stop_rgba(0,    0, 0, 0, 0.5);
      gradient->add_color_stop_rgba(0.33, 1, 1, 1, 0);
      gradient->add_color_stop_rgba(0.66, 0, 0, 0, 0.5);
      gradient->add_color_stop_rgba(1,    1, 1, 1, 0.5);
      context->set_source(gradient);
      context->paint();
    }
  context->restore();
}

}  // namespace Grid
