#ifndef MESSAGE_BOX_H_
#define MESSAGE_BOX_H_

#include <cairomm/context.h>
#include <cairomm/refptr.h>
#include <list>
#include <mutex>

class MessageBox {
 public:
  enum class TextAlign {
    kLeft,
    kCenter,
    kRight
  };

  enum class TextFlow {
    kFromTopToBottom,
    kFromBottomToTop,
  };

  enum class TextWrap {
    kOverflow,
    kCut,
    kWrap,
  };

  MessageBox(double text_r, double text_g, double text_b, double text_a,
             TextAlign text_align, TextFlow text_flow, TextWrap text_wrap);

  void Draw(double x, double y, double width, double height,
            const Cairo::RefPtr<Cairo::Context>& context);

  void AddMessage(const std::string& message);

 private:
  double text_r_, text_g_, text_b_, text_a_;
  TextAlign text_align_;
  TextFlow text_flow_;
  TextWrap text_wrap_;

  std::mutex mutex_;
  std::list<std::string> messages_;
};

#endif  // MESSAGE_BOX_H_
