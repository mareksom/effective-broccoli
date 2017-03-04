#ifndef PAINTER_H_
#define PAINTER_H_

#include <atomic>
#include <cairomm/context.h>
#include <cairomm/surface.h>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <set>
#include <thread>
#include <tuple>

#include "object_updater.h"
#include "object_updater_consume.h"

class Board;
class Viewer;

class Painter {
 public:
  Painter(Board* board, int width, int height);

  void SetViewer(Viewer* viewer);
  void Start();

  const Cairo::RefPtr<Cairo::ImageSurface>* GetAndLockCurrentSurface();
  void ReleaseCurrentSurface();

  void SetTransformation(double x, double y, double scale);

 private:
  std::pair<double, double> BoardToSurfaceCoordinates(double x, double y) const;
  std::pair<double, double> SurfaceToBoardCoordinates(double x, double y) const;

  struct Modification;

  void UpdateCurrentSurface();
  void DrawLoop();
  void ApplyTranslation(double dx, double dy);
  void ApplyModification(const Modification* modification);
  void ProcessAField();

  Board* board_;
  Viewer* viewer_;


  // ----------------------------- Modifications ---------------------------- //

  struct Modification {
    double x;
    double y;
    double scale;
  };

  ObjectUpdaterConsume<Modification> object_updater_consume_;


  // -------------------------------- Drawing ------------------------------- //

  // Drawing thread.
  std::thread thread_;

  int width_, height_;
  double x_, y_;
  double scale_;

  std::set<std::pair<int, int>> fields_to_draw_;

  Cairo::RefPtr<Cairo::ImageSurface> main_surface_;
  Cairo::RefPtr<Cairo::Context> context_;
  Cairo::RefPtr<Cairo::ImageSurface> surfaces[3];
  ObjectUpdater<Cairo::RefPtr<Cairo::ImageSurface>> object_updater_;
};

#endif  // PAINTER_H_
