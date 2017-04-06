#include "object.h"

#include <array>
#include <cassert>
#include <cmath>

namespace {

constexpr double kBorderWidth = 0.2;

void DrawFallback(const Cairo::RefPtr<Cairo::Context>& context) {
  context->rotate(M_PI / 2);
  context->paint();
  const std::string text = ": (";
  Cairo::TextExtents te;
  context->get_text_extents(text, te);
  const double scale = 2 / sqrt(te.width * te.width + te.height * te.height);
  context->scale(scale, scale);
  context->get_text_extents(text, te);
  context->move_to(-te.x_bearing - te.width / 2, -te.y_bearing - te.height / 2);
  context->set_source_rgb(0, 0, 0);
  context->show_text(text);
}

void None(const Cairo::RefPtr<Cairo::Context>& context) {
  // Nothing.
}

void Square(const Cairo::RefPtr<Cairo::Context>& context) {
  constexpr double sqrt_2 = 1.4142135623730951;
  context->rectangle(-sqrt_2 / 2, -sqrt_2 / 2, sqrt_2, sqrt_2);
  context->clip_preserve();
  context->paint();
  context->set_source_rgb(0, 0, 0);
  context->set_line_width(kBorderWidth);
  context->stroke();
}

void Circle(const Cairo::RefPtr<Cairo::Context>& context) {
  context->paint();
  context->arc(0, 0, 1, 0, 2 * M_PI);
  context->set_line_width(kBorderWidth);
  context->set_source_rgb(0, 0, 0);
  context->stroke();
}

void Triangle(const Cairo::RefPtr<Cairo::Context>& context) {
  constexpr double sqrt_3 = 1.7320508075688772;
  context->move_to(0, -1);
  context->line_to(sqrt_3 / 2, 0.5);
  context->line_to(-sqrt_3 / 2, 0.5);
  context->close_path();
  context->clip_preserve();
  context->paint();
  context->set_source_rgb(0, 0, 0);
  context->set_line_width(kBorderWidth);
  context->stroke();
}

void Heart(const Cairo::RefPtr<Cairo::Context>& context) {
  constexpr double asin_0p6 = 0.6435011087932844;
  context->move_to(0, -0.6);
  context->arc(0.4, -0.3, 0.5, -M_PI + asin_0p6, 0);
  context->curve_to(0.9, 0.0, 0.9, 0.0, 0, 1);
  context->curve_to(-0.9, 0.0, -0.9, 0.0, -0.9, -0.3);
  context->arc(-0.4, -0.3, 0.5, M_PI, -asin_0p6);
  context->close_path();
  context->clip_preserve();
  context->paint();
  context->set_source_rgb(0, 0, 0);
  context->set_line_width(kBorderWidth);
  context->stroke();
}

void Potion(const Cairo::RefPtr<Cairo::Context>& context) {
  context->scale(1.015, 1.015);
  context->translate(0, 0.28);
  constexpr double sqrt_3 = 1.7320508075688772;
  constexpr double neck_width = 0.3;
  const double neck_top = 1.2358574958303243;
  // Fill.
  context->move_to(neck_width / 1.6, -0.4);
  context->line_to(0.4 * sqrt_3 - 0.05, 0.2);
  context->line_to(0.4 * sqrt_3, 0.35);
  context->line_to(0.4 * sqrt_3 - 0.1, 0.43);
  context->line_to(-0.4 * sqrt_3 + 0.1, 0.43);
  context->line_to(-0.4 * sqrt_3, 0.35);
  context->line_to(-0.4 * sqrt_3 + 0.05, 0.2);
  context->line_to(-neck_width / 1.6, -0.4);
  context->close_path();
  context->fill();
  context->move_to(neck_width / 1.6, -0.4);
  context->line_to(-neck_width / 1.6, -0.4);
  context->set_line_width(kBorderWidth / 4);
  context->set_source_rgb(0, 0, 0);
  context->stroke();
  // Bubbles.
  context->arc(0, 0.25, 0.08, 0, 2 * M_PI);
  context->stroke();
  context->arc(-0.2, -0.08, 0.07, 0, 2 * M_PI);
  context->set_line_width(kBorderWidth / 6);
  context->stroke();
  context->arc(0.3, -0.02, 0.04, 0, 2 * M_PI);
  context->set_line_width(kBorderWidth / 8);
  context->stroke();
  // Border.
  context->move_to(neck_width / 2, -neck_top);
  context->curve_to(neck_width / 2, -0.2,
                    sqrt_3 / 4, -0.25,
                    0.8 * sqrt_3 / 2, 0.2);
  context->curve_to(sqrt_3 / 2, 0.5,
                    sqrt_3 / 4, 0.5,
                    0, 0.5);
  context->curve_to(-sqrt_3 / 4, 0.5,
                    -sqrt_3 / 2, 0.5,
                    -0.8 * sqrt_3 / 2, 0.2);
  context->curve_to(-sqrt_3 / 4, -0.25,
                    -neck_width / 2, -0.2,
                    -neck_width / 2, -neck_top);
  context->close_path();
  context->clip_preserve();
  context->set_source_rgb(0, 0, 0);
  context->set_line_width(kBorderWidth);
  context->stroke();
}

void Flag(const Cairo::RefPtr<Cairo::Context>& context) {
  context->scale(1.38, 1.38);
  constexpr double stick_length = 1;
  constexpr double flag_width = 0.8;
  constexpr double flag_height = 0.6;
  context->translate(-0.35, -stick_length / 2);
  context->set_line_cap(Cairo::LineCap::LINE_CAP_ROUND);
  context->set_line_join(Cairo::LineJoin::LINE_JOIN_ROUND);
  context->move_to(0, 0);
  context->curve_to(flag_width / 3, flag_height / 8,
                    flag_width * 2 / 3, flag_height / 8,
                    flag_width, 0);
  context->curve_to(flag_width * 7 / 8, flag_height / 3,
                    flag_width * 7 / 8, flag_height * 2 / 3,
                    flag_width, flag_height);
  context->curve_to(flag_width * 2 / 3, flag_height * 9 / 8,
                    flag_width / 3, flag_height * 9 / 8,
                    0, flag_height);
  context->close_path();
  context->fill_preserve();
  context->set_line_width(kBorderWidth / 2);
  context->set_source_rgb(0, 0, 0);
  context->stroke();
  context->move_to(0, 0);
  context->line_to(0, stick_length);
  context->set_line_width(kBorderWidth);
  context->stroke();
}

void Pentagram(const Cairo::RefPtr<Cairo::Context>& context) {
  context->move_to(0, -1);
  context->line_to(cos(M_PI / 2 + 4 * M_PI / 5), -sin(M_PI / 2 + 4 * M_PI / 5));
  context->line_to(cos(M_PI / 2 + 8 * M_PI / 5), -sin(M_PI / 2 + 8 * M_PI / 5));
  context->line_to(cos(M_PI / 2 + 2 * M_PI / 5), -sin(M_PI / 2 + 2 * M_PI / 5));
  context->line_to(cos(M_PI / 2 + 6 * M_PI / 5), -sin(M_PI / 2 + 6 * M_PI / 5));
  context->close_path();
  context->set_fill_rule(Cairo::FillRule::FILL_RULE_EVEN_ODD);
  context->fill_preserve();

  context->set_source_rgb(0, 0, 0);
  context->set_line_width(kBorderWidth / 2);
  context->stroke();

  context->arc(0, 0, 1, 0, 2 * M_PI);
  context->set_line_width(kBorderWidth);
  context->stroke();
}

void Smile(const Cairo::RefPtr<Cairo::Context>& context) {
  context->paint();
  context->set_source_rgb(0, 0, 0);
  context->set_line_cap(Cairo::LineCap::LINE_CAP_ROUND);
  context->set_line_width(kBorderWidth / 2);
  context->arc(0, 0, 0.5, M_PI / 8, M_PI * 7 / 8);
  context->stroke();
  context->arc(0.3, -0.3, kBorderWidth / 2, 0, 2 * M_PI);
  context->fill();
  context->arc(-0.3, -0.3, kBorderWidth / 2, 0, 2 * M_PI);
  context->fill();
  context->set_line_width(kBorderWidth);
  context->arc(0, 0, 1, 0, 2 * M_PI);
  context->stroke();
}

void Sad(const Cairo::RefPtr<Cairo::Context>& context) {
  context->paint();
  context->set_source_rgb(0, 0, 0);
  context->set_line_cap(Cairo::LineCap::LINE_CAP_ROUND);
  context->set_line_width(kBorderWidth / 2);
  context->arc(0, 0.6, 0.5, -M_PI * 7 / 8, -M_PI / 8);
  context->stroke();
  context->arc(0.3, -0.3, kBorderWidth / 2, 0, 2 * M_PI);
  context->fill();
  context->arc(-0.3, -0.3, kBorderWidth / 2, 0, 2 * M_PI);
  context->fill();
  context->set_line_width(kBorderWidth);
  context->arc(0, 0, 1, 0, 2 * M_PI);
  context->stroke();
}

void Coin(const Cairo::RefPtr<Cairo::Context>& context) {
  context->scale(1, -1);
  context->paint();
  context->set_source_rgb(0, 0, 0);

  constexpr double radius = 1.0 / 3;

  context->set_line_cap(Cairo::LineCap::LINE_CAP_ROUND);
  context->set_line_width(kBorderWidth / 2);
  context->arc(0, radius, radius, M_PI / 4, M_PI * 3 / 2);
  context->arc_negative(0, -radius, radius, M_PI / 2, M_PI * 5 / 4);
  context->stroke();

  context->move_to(radius / 3, -2.5 * radius);
  context->line_to(radius / 3, 2.5 * radius);
  context->stroke();
  context->move_to(-radius / 3, -2.5 * radius);
  context->line_to(-radius / 3, 2.5 * radius);
  context->stroke();

  context->set_line_width(kBorderWidth);
  context->arc(0, 0, 1, 0, 2 * M_PI);
  context->stroke();
}

std::array<void(*)(const Cairo::RefPtr<Cairo::Context>&),
           static_cast<int>(Object::kCount)> object_drawers = {
      None,
      Square,
      Circle,
      Triangle,
      Heart,
      Potion,
      Flag,
      Pentagram,
      Smile,
      Sad,
      Coin,
    };

}  // namespace

int MakeObject(Object object, int r, int g, int b) {
  assert(0 <= static_cast<int>(Object::kCount));
  assert(static_cast<int>(Object::kCount) < 256);
  assert(0 <= r and r < 256);
  assert(0 <= g and g < 256);
  assert(0 <= b and b < 256);
  const int object_id = static_cast<int>(object);
  return (object_id << 24) ^ (r << 16) ^ (g << 8) ^ b;
}

void DrawObject(const Cairo::RefPtr<Cairo::Context>& context, int object) {
  const int b = object & 255;
  const int g = (object >> 8) & 255;
  const int r = (object >> 16) & 255;
  const int object_id = (object >> 24) & 255;
  context->save();
    context->set_source_rgb(r / 255.0, g / 255.0, b / 255.0);
    context->arc(0, 0, 1, 0, 2 * M_PI);
    context->clip();
    const int count = static_cast<int>(Object::kCount);
    if (0 <= object_id and object_id < count) {
      object_drawers[object_id](context);
    } else {
      DrawFallback(context);
    }
  context->restore();
}
