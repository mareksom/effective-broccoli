#include "message_box.h"

MessageBox::MessageBox(
    double text_r, double text_g, double text_b, double text_a,
    TextAlign text_align, TextFlow text_flow, TextWrap text_wrap)
    : text_r_(text_r), text_g_(text_g), text_b_(text_b), text_a_(text_a),
      text_align_(text_align), text_flow_(text_flow), text_wrap_(text_wrap) {}

void MessageBox::Draw(double x, double y, double width, double height,
                      const Cairo::RefPtr<Cairo::Context>& context) {
  std::lock_guard<std::mutex> lock(mutex_);
  context->save();
    context->translate(x, y);

    double clip_min_x, clip_max_x, clip_min_y, clip_max_y;
    context->get_clip_extents(clip_min_x, clip_min_y, clip_max_x, clip_max_y);
    context->rectangle(clip_min_x, 0.0,
                       clip_max_x - clip_min_x, height);
    context->clip();

    context->set_source_rgba(text_r_, text_g_, text_b_, text_a_);
    double bg_col, bg_a;
    if (text_r_ + text_g_ + text_b_ > 1.5) {
      bg_col = 0.0;
    } else {
      bg_col = 1.0;
    }
    bg_a = text_a_ * 0.3;

    Cairo::FontExtents fe;
    context->get_font_extents(fe);

    const int flow_direction =
        (text_flow_ == TextFlow::kFromTopToBottom ? 1 : -1);

    // Splits the given message into two parts:
    //   1) The part that fits in one line and is the prefix of the @message.
    //   2) The rest of the message.
    // If @dots is true, the rest of the message will be empty and the first
    // part will end with three dots (...).
    auto SplitMessage = [this, &context, &fe, width](
        const std::string& message, bool dots)
            -> std::pair<std::string, std::string> {
      Cairo::TextExtents te;
      context->get_text_extents(message, te);
      if (te.x_bearing + te.width <= width) {
        return std::make_pair(message, std::string());
      }
      int bin_min = 1, bin_max = static_cast<int>(message.size());
      if (dots or message.empty()) {
        bin_min = 0;
      }
      while (bin_min < bin_max) {
        const int bin_mid = (bin_min + bin_max + 1) / 2;
        std::string part = message.substr(0, bin_mid);
        if (dots) {
          part += "...";
        }
        context->get_text_extents(part, te);
        if (te.x_bearing + te.width <= width) {
          bin_min = bin_mid;
        } else {
          bin_max = bin_mid - 1;
        }
      }
      std::pair<std::string, std::string> result;
      result.first = message.substr(0, bin_min);
      if (dots) {
        result.first += "...";
      } else {
        result.second = message.substr(bin_min);
      }
      return std::move(result);
    };

    // Draws a @message at height &y and returns the new height.
    auto DrawMessage = [this, &context, &fe, flow_direction, width,
                        &bg_col, &bg_a](
        double y, const std::string& message) -> double {
      context->save();
        Cairo::TextExtents te;
        context->get_text_extents(message, te);
        switch (text_align_) {
          case TextAlign::kLeft:
            context->translate(0, y);
            break;
          case TextAlign::kCenter:
            context->translate((width - te.width) / 2 - te.x_bearing, y);
            break;
          case TextAlign::kRight:
            context->translate(width - te.width - te.x_bearing, y);
            break;
        }
        context->save();
          context->rectangle(-fe.height / 4, -fe.ascent,
                             te.x_bearing + te.width + fe.height / 2,
                             fe.height);
          context->set_source_rgba(bg_col, bg_col, bg_col, bg_a);
          context->fill();
        context->restore();
        context->move_to(0, 0);
        context->show_text(message);
      context->restore();
      return y + flow_direction * fe.height;
    };

    // Fits, then draws a @message at height @y and returns the new height.
    auto FitMessage = [this, &context, &fe, flow_direction,
                       &SplitMessage, &DrawMessage](
      double y, const std::string& message) -> double {
      switch (text_wrap_) {
        case TextWrap::kOverflow: {
          return DrawMessage(y, message);
        }

        case TextWrap::kCut: {
          auto split = SplitMessage(message, true /* dots */);
          return DrawMessage(y, split.first);
        }

        case TextWrap::kWrap: {
          std::pair<std::string, std::string> rest;
          rest.second = message;
          std::vector<std::string> parts;
          do {
            rest = SplitMessage(rest.second, false /* dots */);
            parts.push_back(rest.first);
          } while (!rest.second.empty());
          if (text_flow_ == TextFlow::kFromBottomToTop) {
            std::reverse(parts.begin(), parts.end());
          }
          for (const std::string& part : parts) {
            y = DrawMessage(y, part);
          }
          return y;
        }
      }
    };

    double current_y = (text_flow_ == TextFlow::kFromTopToBottom ?
        fe.ascent : height - fe.descent);
    for (auto it = messages_.begin(); it != messages_.end(); ++it) {
      current_y = FitMessage(current_y, *it);
      if (current_y > height + fe.height or current_y < -fe.height) {
        messages_.erase(std::next(it), messages_.end());
        break;
      }
    }
  context->restore();
}

void MessageBox::AddMessage(const std::string& message) {
  std::lock_guard<std::mutex> lock(mutex_);
  std::string next_line;
  for (char c : message) {
    if (c == '\n') {
      if (!next_line.empty()) {
        messages_.push_front(std::move(next_line));
        next_line.clear();
      }
    } else if (c == '\t') {
      next_line += "  ";
    } else {
      next_line += c;
    }
  }
  if (!next_line.empty()) {
    messages_.push_front(std::move(next_line));
  }
}
