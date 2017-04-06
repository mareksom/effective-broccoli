#include "viewer.h"

#include <cmath>

#include "controller.h"
#include "makra.h"
#include "options.h"
#include "painter.h"

Viewer::Viewer(const Options* options, Painter* painter)
    : options_(options), painter_(painter) {
  add_events(Gdk::BUTTON_PRESS_MASK);
  add_events(Gdk::BUTTON_RELEASE_MASK);
  add_events(Gdk::KEY_PRESS_MASK);
  add_events(Gdk::SMOOTH_SCROLL_MASK);
  add_events(Gdk::STRUCTURE_MASK);
  set_can_focus(true);
  grab_focus();
  redraw_signal_.connect(
      [this]() -> void {
        queue_draw();
      });
}

void Viewer::Redraw() {
  redraw_signal_.emit();
}

bool Viewer::on_draw(const Cairo::RefPtr<Cairo::Context>& context) {
  const int width = get_allocated_width();
  const int height = get_allocated_height();
  const Painter::SurfaceBuffer* surface_buffer =
      painter_->GetAndLockCurrentSurfaceBuffer();
  context->save();
    context->set_source(surface_buffer->surface,
                        surface_buffer->start_x, surface_buffer->start_y);
    context->paint();
  context->restore();
  painter_->ReleaseCurrentSurfaceBuffer();
  context->save();
    options().controller()->Draw(width, height, context);
  context->restore();
  return true;
}

bool Viewer::on_scroll_event(GdkEventScroll* event) {
  if (event->state & GDK_CONTROL_MASK) {
    if (event->direction == GDK_SCROLL_SMOOTH) {
      painter_->Zoom(event->x, event->y,
                     std::pow(options().ZoomSpeed(), -event->delta_y));
    }
  } else {
    if (event->direction == GDK_SCROLL_SMOOTH) {
      painter_->Translate(-event->delta_x * options().ScrollSpeed(),
                          -event->delta_y * options().ScrollSpeed());
    }
  }
  return true;
}

bool Viewer::on_configure_event(GdkEventConfigure* event) {
  painter_->Resize(event->width, event->height);
  return true;
}

bool Viewer::on_button_press_event(GdkEventButton* event) {
  press_button_ = event->button;
  press_point_ = painter_->WindowToBoardCoordinates(event->x, event->y);
  return true;
}

bool Viewer::on_button_release_event(GdkEventButton* event) {
  if (press_button_ == event->button and
      press_point_ == painter_->WindowToBoardCoordinates(event->x, event->y)) {
    options().controller()->FieldClick(
        press_point_.first, press_point_.second, press_button_);
  }
  return true;
}

namespace {

std::string KeyToString(int keyval) {
  switch (keyval) {
    case GDK_KEY_Escape:            return "escape";
    case GDK_KEY_Insert:            return "insert";
    case GDK_KEY_Delete:            return "delete";
    case GDK_KEY_Home:              return "home";
    case GDK_KEY_End:               return "end";
    case GDK_KEY_Page_Up:           return "page_up";
    case GDK_KEY_Page_Down:         return "page_down";
    case GDK_KEY_BackSpace:         return "backspace";
    case GDK_KEY_Tab:               return "tab";
    case GDK_KEY_Return:            return "enter";

    case GDK_KEY_Left:              return "left";
    case GDK_KEY_Right:             return "right";
    case GDK_KEY_Up:                return "up";
    case GDK_KEY_Down:              return "down";

    case GDK_KEY_Shift_L:           return "shift";
    case GDK_KEY_Shift_R:           return "shift";
    case GDK_KEY_Alt_L:             return "alt";
    case GDK_KEY_Alt_R:             return "alt";
    case GDK_KEY_Control_L:         return "ctrl";
    case GDK_KEY_Control_R:         return "ctrl";
    case GDK_KEY_Super_L:           return "windows";
    case GDK_KEY_Super_R:           return "windows";
    case GDK_KEY_ISO_Level3_Shift:  return "alg_gr";

    case GDK_KEY_asciitilde:        return "~";
    case GDK_KEY_grave:             return "`";
    case GDK_KEY_exclam:            return "!";
    case GDK_KEY_at:                return "@";
    case GDK_KEY_numbersign:        return "#";
    case GDK_KEY_dollar:            return "$";
    case GDK_KEY_percent:           return "%";
    case GDK_KEY_asciicircum:       return "^";
    case GDK_KEY_ampersand:         return "&";
    case GDK_KEY_asterisk:          return "*";
    case GDK_KEY_parenleft:         return "(";
    case GDK_KEY_parenright:        return ")";
    case GDK_KEY_minus:             return "-";
    case GDK_KEY_underscore:        return "_";
    case GDK_KEY_plus:              return "+";
    case GDK_KEY_equal:             return "=";
    case GDK_KEY_bracketleft:       return "[";
    case GDK_KEY_bracketright:      return "]";
    case GDK_KEY_braceleft:         return "{";
    case GDK_KEY_braceright:        return "}";
    case GDK_KEY_backslash:         return "\\";
    case GDK_KEY_bar:               return "|";
    case GDK_KEY_colon:             return ":";
    case GDK_KEY_semicolon:         return ";";
    case GDK_KEY_apostrophe:        return "'";
    case GDK_KEY_quotedbl:          return "\"";
    case GDK_KEY_comma:             return ",";
    case GDK_KEY_less:              return "<";
    case GDK_KEY_period:            return ".";
    case GDK_KEY_greater:           return ">";
    case GDK_KEY_slash:             return "/";
    case GDK_KEY_question:          return "?";
    case GDK_KEY_space:             return " ";

    case GDK_KEY_A:                 return "A";
    case GDK_KEY_B:                 return "B";
    case GDK_KEY_C:                 return "C";
    case GDK_KEY_D:                 return "D";
    case GDK_KEY_E:                 return "E";
    case GDK_KEY_F:                 return "F";
    case GDK_KEY_G:                 return "G";
    case GDK_KEY_H:                 return "H";
    case GDK_KEY_I:                 return "I";
    case GDK_KEY_J:                 return "J";
    case GDK_KEY_K:                 return "K";
    case GDK_KEY_L:                 return "L";
    case GDK_KEY_M:                 return "M";
    case GDK_KEY_N:                 return "N";
    case GDK_KEY_O:                 return "O";
    case GDK_KEY_P:                 return "P";
    case GDK_KEY_Q:                 return "Q";
    case GDK_KEY_R:                 return "R";
    case GDK_KEY_S:                 return "S";
    case GDK_KEY_T:                 return "T";
    case GDK_KEY_U:                 return "U";
    case GDK_KEY_V:                 return "V";
    case GDK_KEY_W:                 return "W";
    case GDK_KEY_X:                 return "X";
    case GDK_KEY_Y:                 return "Y";
    case GDK_KEY_Z:                 return "Z";

    case GDK_KEY_a:                 return "a";
    case GDK_KEY_b:                 return "b";
    case GDK_KEY_c:                 return "c";
    case GDK_KEY_d:                 return "d";
    case GDK_KEY_e:                 return "e";
    case GDK_KEY_f:                 return "f";
    case GDK_KEY_g:                 return "g";
    case GDK_KEY_h:                 return "h";
    case GDK_KEY_i:                 return "i";
    case GDK_KEY_j:                 return "j";
    case GDK_KEY_k:                 return "k";
    case GDK_KEY_l:                 return "l";
    case GDK_KEY_m:                 return "m";
    case GDK_KEY_n:                 return "n";
    case GDK_KEY_o:                 return "o";
    case GDK_KEY_p:                 return "p";
    case GDK_KEY_q:                 return "q";
    case GDK_KEY_r:                 return "r";
    case GDK_KEY_s:                 return "s";
    case GDK_KEY_t:                 return "t";
    case GDK_KEY_u:                 return "u";
    case GDK_KEY_v:                 return "v";
    case GDK_KEY_w:                 return "w";
    case GDK_KEY_x:                 return "x";
    case GDK_KEY_y:                 return "y";
    case GDK_KEY_z:                 return "z";

    case GDK_KEY_0:                 return "0";
    case GDK_KEY_1:                 return "1";
    case GDK_KEY_2:                 return "2";
    case GDK_KEY_3:                 return "3";
    case GDK_KEY_4:                 return "4";
    case GDK_KEY_5:                 return "5";
    case GDK_KEY_6:                 return "6";
    case GDK_KEY_7:                 return "7";
    case GDK_KEY_8:                 return "8";
    case GDK_KEY_9:                 return "9";

    case GDK_KEY_F1:                return "f1";
    case GDK_KEY_F2:                return "f2";
    case GDK_KEY_F3:                return "f3";
    case GDK_KEY_F4:                return "f4";
    case GDK_KEY_F5:                return "f5";
    case GDK_KEY_F6:                return "f6";
    case GDK_KEY_F7:                return "f7";
    case GDK_KEY_F8:                return "f8";
    case GDK_KEY_F9:                return "f9";
    case GDK_KEY_F10:               return "f10";
    case GDK_KEY_F11:               return "f11";
    case GDK_KEY_F12:               return "f12";

    default:                        return "(" + std::to_string(keyval) + ")";
  }
}

}  // namespace

bool Viewer::on_key_press_event(GdkEventKey* event) {
  options().controller()->KeyPress(KeyToString(event->keyval));
  return true;
}

const Options& Viewer::options() const {
  return *options_;
}
