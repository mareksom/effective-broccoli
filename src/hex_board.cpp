#include "hex_board.h"

#include <string>

#include "controller.h"
#include "object.h"
#include "options.h"

namespace {

constexpr double sin_pi_div_6 = 0.5;
constexpr double sin_pi_div_3 = 0.8660254037844386;

//          _,-+-,_     _,-+-,_
//       ,-'   |   `-+-'   |   `-,
//       |     |     |     |     |
//       +-----+-----+-----+-----|  -,
//       |     |     |     |     |   |
//    _,-+-,_  |  _,-+-,_  |  _,-'    > rectangle_height
// ,-'   |   `-+-'   |   `-+-'       |
// |     |     |     |     |         |
// |-----+-----+-----+-----|        -'
// |     |     |     |     |
// `-,_  |  _,-'-,_  |  _,-'
//     `-+-'       `-+-'    _\
//                       _+'
//       '--,--'       \'  `-> 1
//          v
//         rectangle_width

constexpr double rectangle_width = sin_pi_div_3;
constexpr double rectangle_height = sin_pi_div_6 + 1;

}  // namespace

std::pair<int, int> HexBoard::PointToCoordinates(double x, double y) const {
  int rx = static_cast<int>(std::floor(x / rectangle_width));
  int ry = static_cast<int>(std::floor(y / rectangle_height));
  const int r = ((rx + ry) % 2 + 2) % 2;
  const int dif = !(rectangle_width * (2 * x - (2 * rx + 1) * rectangle_width)
      + ((1 - 2 * r) * rectangle_height)
      * (2 * y - rectangle_height * (2 * ry + 1)) < 0);
  ry += r - (2 * r - 1) * dif;
  rx = (dif + rx - ry) / 2;
  return std::make_pair(rx, ry);
}

void HexBoard::IterateFieldsInRectangle(
    double x_min, double y_min, double x_max, double y_max,
    std::function<void(int, int)> callback) const {
  const int rx = static_cast<int>(std::floor(x_min / rectangle_width));
  const int ry = static_cast<int>(std::floor(y_min / rectangle_height));
  const int rx2 = static_cast<int>(std::floor(x_max / rectangle_width));
  const int ry2 = static_cast<int>(std::floor(y_max / rectangle_height));
  const int width = options.controller()->BoardWidth();
  const int height = options.controller()->BoardHeight();
  for (int x = rx; x <= rx2; x++) {
    for (int y = ry; y <= ry2 + 1; y++) {
      if (0 <= x and x < width) {
        if (0 <= y and y < height) {
          callback((x - y + ((x + y) % 2 + 2) % 2) / 2, y);
        }
      }
    }
  }
}

void HexBoard::DrawField(
    int x, int y, const Cairo::RefPtr<Cairo::Context>& context) const {
  const int color = options.controller()->GetFieldColor(x, y);
  context->save();
    context->translate((2 * x + y) * sin_pi_div_3, y * (sin_pi_div_6 + 1));
    context->move_to(0, -1);
    context->line_to(sin_pi_div_3, -sin_pi_div_6);
    context->line_to(sin_pi_div_3, sin_pi_div_6);
    context->line_to(0, 1);
    context->line_to(-sin_pi_div_3, sin_pi_div_6);
    context->line_to(-sin_pi_div_3, -sin_pi_div_6);
    context->close_path();
    context->clip_preserve();
    context->set_source_rgb(
        GetDoubleR(color), GetDoubleG(color), GetDoubleB(color));
    context->fill_preserve();
    context->set_source_rgb(0, 0, 0);
    context->set_line_width(0.01);
    context->stroke();
    // Text.
    context->move_to(-sin_pi_div_3, 0);
    context->set_font_size(0.25);
    context->show_text(
        "(" + std::to_string(x) + ", " + std::to_string(y) + ")");
    // Object.
    context->save();
      context->scale(0.8, 0.8);
      DrawObject(context, options.controller()->GetObject(x, y));
    context->restore();
  context->restore();
}
