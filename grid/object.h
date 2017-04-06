#ifndef GRID_OBJECT_H_
#define GRID_OBJECT_H_

#include <cairomm/context.h>
#include <cairomm/refptr.h>

namespace Grid {

enum class Object : int {
  kNone      = 0,
  kSquare    = 1,
  kCircle    = 2,
  kTriangle  = 3,
  kHeart     = 4,
  kPotion    = 5,
  kFlag      = 6,
  kPentagram = 7,
  kSmile     = 8,
  kSad       = 9,
  kCoin      = 10,

  // Not actually an object.
  kCount = 11,
};

int MakeObject(Object object, int r, int g, int b);

void DrawObject(const Cairo::RefPtr<Cairo::Context>& context, int object);

}  // namespace Grid

#endif  // GRID_OBJECT_H_
