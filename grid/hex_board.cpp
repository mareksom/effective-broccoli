#include "hex_board.h"

#include <cassert>
#include <string>

#include "controller.h"
#include "object.h"
#include "options.h"

namespace Grid {

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

std::pair<double, double> HexBoard::CenterOfField(int x, int y) const {
  return std::make_pair((2 * x + y) * rectangle_width, y * rectangle_height);
}

void HexBoard::IterateFieldsInRectangle(
    double x_min, double y_min, double x_max, double y_max,
    std::function<void(int, int)> callback) const {
  const int rx = static_cast<int>(std::floor(x_min / rectangle_width));
  const int ry = static_cast<int>(std::floor(y_min / rectangle_height));
  const int rx2 = static_cast<int>(std::floor(x_max / rectangle_width));
  const int ry2 = static_cast<int>(std::floor(y_max / rectangle_height));
  const int width = options().controller()->BoardWidth();
  const int height = options().controller()->BoardHeight();
  for (int y = std::max(ry, 0); y <= ry2 + 1 and y < height; y++) {
    for (int x = std::max(rx, y - 1); x <= rx2 and x < 2 * width + y - 1; x++) {
      const int actual_x = (x - y + ((x + y) % 2 + 2) % 2) / 2;
      const int actual_y = y;
      assert(0 <= actual_y and actual_y < height);
      if (0 <= actual_x and actual_x < width) {
        callback(actual_x, actual_y);
      }
    }
  }
}

void HexBoard::DrawField(
    int x, int y, const Cairo::RefPtr<Cairo::Context>& context) const {
  const int color = options().controller()->GetFieldColor(x, y);
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
    // Object.
    context->save();
      context->scale(0.8, 0.8);
      DrawObject(context, options().controller()->GetObject(x, y));
    context->restore();
  context->restore();
}

}  // namespace Grid
