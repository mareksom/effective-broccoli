#ifndef BOARD_H_
#define BOARD_H_

#include <cairomm/context.h>
#include <cairomm/refptr.h>
#include <functional>
#include <tuple>
#include <utility>

class Board {
 public:
  virtual ~Board() = default;

  virtual std::pair<int, int> PointToCoordinates(double x, double y) const = 0;

  virtual void IterateFieldsInRectangle(
      double x_min, double y_min, double x_max, double y_max,
      std::function<void(int, int)> callback) const = 0;

  virtual void DrawField(
      int x, int y, const Cairo::RefPtr<Cairo::Context>& context) const = 0;
};

#endif  // BOARD_H_
