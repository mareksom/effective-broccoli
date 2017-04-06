#include "single_message_box.h"

namespace Grid {

SingleMessageBox::SingleMessageBox(
    double text_r, double text_g, double text_b, double text_a,
    std::function<void(StreamReader&)> generator)
    : text_r_(text_r), text_g_(text_g), text_b_(text_b), text_a_(text_a),
      generator_(generator) {}

void SingleMessageBox::Draw(double x, double y, double height,
                            const Cairo::RefPtr<Cairo::Context>& context) {
  context->save();
    context->translate(x, y);
    double bg_col, bg_a;
    if (text_r_ + text_g_ + text_b_ < 1.5) {
      bg_col = 1.0;
    } else {
      bg_col = 0.0;
    }
    bg_a = text_a_ * 0.2;
    const std::string& message = GetMessage();
    if (!message.empty()) {
      Cairo::TextExtents te;
      context->get_text_extents(message, te);
      const double scale = (height / te.height) * 0.9;
      const double new_height = height / scale;
      const double border = (new_height - te.height) / 2;
      context->scale(scale, scale);
      context->get_text_extents(message, te);
      const double y_space = new_height - te.height;
      // Background.
      context->rectangle(-te.width - 2 * border, 0,
                         te.width + 2 * border, new_height);
      context->set_source_rgba(bg_col, bg_col, bg_col, bg_a);
      context->fill();
      // Text.
      context->set_source_rgba(text_r_, text_g_, text_b_, text_a_);
      context->move_to(-te.width - border - te.x_bearing,
                       y_space / 2 - te.y_bearing);
      context->show_text(message);
    }
  context->restore();
}

std::string SingleMessageBox::GetMessage() {
  std::string message;
  {
    StreamReader sr([&message](const std::string& m) -> void {
                      message = m;
                    });;
    generator_(sr);
  }
  return message;
}

}  // namespace Grid
