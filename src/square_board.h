#ifndef SQUARE_BOARD_H_
#define SQUARE_BOARD_H_

#include "board.h"

class SquareBoard : public Board {
 public:
  std::pair<int, int> PointToCoordinates(double x, double y) const override;

  void IterateFieldsInRectangle(
      double x_min, double y_min, double x_max, double y_max,
      std::function<void(int, int)> callback) const override;

  void DrawField(int x, int y,
                 const Cairo::RefPtr<Cairo::Context>& context) const override;
};

#endif  // SQUARE_BOARD_H_
