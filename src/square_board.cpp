#include "square_board.h"

#include <string>

std::pair<int, int> SquareBoard::PointToCoordinates(
    double x, double y) const {
  return std::make_pair(static_cast<int>(std::floor(x)),
                        static_cast<int>(std::floor(y)));
}

void SquareBoard::IterateFieldsInRectangle(
    double x_min, double y_min, double x_max, double y_max,
    std::function<void(int, int)> callback) const {
  const int x_min_int = static_cast<int>(std::floor(x_min));
  const int y_min_int = static_cast<int>(std::floor(y_min));
  const int x_max_int = static_cast<int>(std::floor(x_max));
  const int y_max_int = static_cast<int>(std::floor(y_max));
  for (int x = x_min_int; x <= x_max_int; x++) {
    for (int y = y_min_int; y <= y_max_int; y++) {
      callback(x, y);
    }
  }
}

void SquareBoard::DrawField(
    int x, int y, const Cairo::RefPtr<Cairo::Context>& context) const {
  context->save();
    context->move_to(x, y);
    context->line_to(x + 1, y);
    context->line_to(x + 1, y + 1);
    context->line_to(x, y + 1);
    context->close_path();
    context->set_source_rgb(1, 1, 1);
    context->fill_preserve();
    context->set_source_rgb(0, 0, 0);
    context->set_line_width(0.01);
    context->stroke();
    // Text.
    context->move_to(x, y + 0.5);
    context->set_font_size(0.25);
    context->show_text(
        "(" + std::to_string(x) + ", " + std::to_string(y) + ")");
  context->restore();
}
