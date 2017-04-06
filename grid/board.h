#ifndef GRID_BOARD_H_
#define GRID_BOARD_H_

#include <cairomm/context.h>
#include <cairomm/refptr.h>
#include <functional>
#include <tuple>
#include <utility>

#include "options.h"

namespace Grid {

class Board {
 public:
  Board();

  virtual ~Board() = default;

  void SetOptions(const Options* options);

  virtual std::pair<int, int> PointToCoordinates(double x, double y) const = 0;

  virtual std::pair<double, double> CenterOfField(int x, int y) const = 0;

  virtual void IterateFieldsInRectangle(
      double x_min, double y_min, double x_max, double y_max,
      std::function<void(int, int)> callback) const = 0;

  virtual void DrawField(
      int x, int y, const Cairo::RefPtr<Cairo::Context>& context) const = 0;

  const Options& options() const;

 private:
  const Options* options_;
};

}  // namespace Grid

#endif  // GRID_BOARD_H_
