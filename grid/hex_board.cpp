#include "hex_board.h"

#include <cassert>
#include <string>

#include "controller.h"
#include "makra.h"
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
  int board_x_min, board_y_min, board_x_max, board_y_max;
  options().controller()->GetExtensions(
      board_x_min, board_y_min, board_x_max, board_y_max);
  int rx = static_cast<int>(std::floor(x_min / (2 * rectangle_width)));
  int rx2 = static_cast<int>(std::floor(x_max / (2 * rectangle_width)));
  int ry = static_cast<int>(std::floor(y_min / rectangle_height)) - 1;
  int ry2 = static_cast<int>(std::floor(y_max / rectangle_height)) + 1;
  ry = std::max(ry, board_y_min);
  ry2 = std::min(ry2, board_y_max);
  rx -= ry / 2 + 2;
  rx2 -= ry / 2 - 2;
  for (int y = ry; y <= ry2; y++) {
    const int new_rx = std::max(board_x_min, rx);
    const int new_rx2 = std::min(board_x_max, rx2);
    for (int x = new_rx; x <= new_rx2; x++) {
      callback(x, y);
    }
    if (y % 2 == 0) {
      rx--;
      rx2--;
    }
  }
}

void HexBoard::DrawField(
    int x, int y, const Cairo::RefPtr<Cairo::Context>& context) const {
  bool border;
  int color, object;
  std::string text;
  bool fog;
  options().controller()->GetFieldInfo(x, y, border, color, object, text, fog);
  context->save();
    context->translate((2 * x + y) * sin_pi_div_3, y * (sin_pi_div_6 + 1));
    // Background.
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
    context->fill();
    // Object.
    context->save();
      context->scale(0.8, 0.8);
      DrawObject(context, object);
    context->restore();
    if (!text.empty()) {
      context->save();
        Cairo::TextExtents te;
        context->get_text_extents(text, te);
        constexpr double text_size = 0.7;
        const double scale = std::min(text_size / te.width, text_size / te.height);
        context->scale(scale, scale);
        context->get_text_extents(text, te);
        // Background.
        constexpr double ctg_60 = 0.5773502691896258;
        constexpr double border_ratio = 0.05;
        const double width = 1 / scale;
        const double border = width * border_ratio;
        const double lift = te.width / 2 * ctg_60;
        context->rectangle(
            -te.width / 2 - border, width - lift - te.height - 2 * border,
            te.width + 2 * border, te.height + 2 * border);
        context->set_source_rgba(1, 1, 1, 0.6);
        context->fill();
        // Text.
        context->move_to(-te.width / 2 - te.x_bearing,
                         width - lift - te.height - te.y_bearing - border);
        context->set_source_rgba(0, 0, 0, 0.6);
        context->show_text(text);
      context->restore();
    }
    // Border.
    if (border) {
      context->move_to(0, -1);
      context->line_to(sin_pi_div_3, -sin_pi_div_6);
      context->line_to(sin_pi_div_3, sin_pi_div_6);
      context->line_to(0, 1);
      context->line_to(-sin_pi_div_3, sin_pi_div_6);
      context->line_to(-sin_pi_div_3, -sin_pi_div_6);
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
