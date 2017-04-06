#ifndef GRID_SINGLE_MESSAGE_BOX_H_
#define GRID_SINGLE_MESSAGE_BOX_H_

#include <cairomm/context.h>
#include <cairomm/refptr.h>
#include <functional>

#include "stream_reader.h"

namespace Grid {

class SingleMessageBox {
 public:
  SingleMessageBox(double text_r, double text_g, double text_b, double text_a,
                   std::function<void(StreamReader&)> generator);

  // (x, y) -- top right corner of the message box.
  void Draw(double x, double y, double height,
            const Cairo::RefPtr<Cairo::Context>& context);

 private:
  std::string GetMessage();

  double text_r_, text_g_, text_b_, text_a_;
  std::function<void(StreamReader&)> generator_;
};

}  // namespace Grid

#endif  // GRID_SINGLE_MESSAGE_BOX_H_
