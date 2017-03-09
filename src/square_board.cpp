#include "square_board.h"

#include <algorithm>
#include <cassert>
#include <string>

#include "controller.h"
#include "makra.h"
#include "object.h"
#include "options.h"

std::pair<int, int> SquareBoard::PointToCoordinates(
    double x, double y) const {
  return std::make_pair(static_cast<int>(std::floor(x)),
                        static_cast<int>(std::floor(y)));
}

void SquareBoard::IterateFieldsInRectangle(
    double x_min, double y_min, double x_max, double y_max,
    std::function<void(int, int)> callback) const {
  int x_min_int = static_cast<int>(std::floor(x_min));
  int y_min_int = static_cast<int>(std::floor(y_min));
  int x_max_int = static_cast<int>(std::floor(x_max));
  int y_max_int = static_cast<int>(std::floor(y_max));
  x_min_int = std::max(x_min_int, 0);
  y_min_int = std::max(y_min_int, 0);
  x_max_int = std::min(x_max_int, options.controller()->BoardWidth() - 1);
  y_max_int = std::min(y_max_int, options.controller()->BoardHeight() - 1);
  for (int x = x_min_int; x <= x_max_int; x++) {
    for (int y = y_min_int; y <= y_max_int; y++) {
      callback(x, y);
    }
  }
}

void SquareBoard::DrawField(
    int x, int y, const Cairo::RefPtr<Cairo::Context>& context) const {
  const int color = options.controller()->GetFieldColor(x, y);
  context->save();
    context->translate(x + 0.5, y + 0.5);
    context->move_to(-0.5, -0.5);
    context->line_to(0.5, -0.5);
    context->line_to(0.5, 0.5);
    context->line_to(-0.5, 0.5);
    context->close_path();
    context->clip_preserve();
    context->set_source_rgb(
        GetDoubleR(color), GetDoubleG(color), GetDoubleB(color));
    context->fill_preserve();
    context->set_source_rgb(0, 0, 0);
    context->set_line_width(0.01);
    context->stroke();
    // Text.
    context->move_to(-0.5, 0);
    context->set_font_size(0.25);
    context->show_text(
        "(" + std::to_string(x) + ", " + std::to_string(y) + ")");
    // Object.
    context->save();
      context->scale(0.4, 0.4);
      DrawObject(context, options.controller()->GetObject(x, y));
    context->restore();
  context->restore();
}
