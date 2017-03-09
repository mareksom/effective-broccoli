#ifndef OBJECT_H_
#define OBJECT_H_

#include <cairomm/context.h>
#include <cairomm/refptr.h>

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
  kCoin     = 9,

  // Not actually an object.
  kCount = 10,
};

int MakeObject(Object object, int r, int g, int b);

void DrawObject(const Cairo::RefPtr<Cairo::Context>& context, int object);

#endif  // OBJECT_H_
