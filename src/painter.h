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
  struct SurfaceBuffer {
    Cairo::RefPtr<Cairo::ImageSurface> surface;
    double start_x;
    double start_y;
  };

  Painter(Board* board, int width, int height);

  void SetViewer(Viewer* viewer);
  void Start();

  const SurfaceBuffer* GetAndLockCurrentSurfaceBuffer();
  void ReleaseCurrentSurfaceBuffer();

  void Resize(int width, int height);
  void Translate(double dx, double dy);
  void Zoom(double x, double y, double factor);

 private:
  struct Modification {
    double tx;
    double ty;
    double scale;
    int width;
    int height;
  };

  std::pair<double, double> BoardToSurfaceCoordinates(double x, double y) const;
  std::pair<double, double> SurfaceToBoardCoordinates(double x, double y) const;

  void SetModification();
  void UpdateCurrentSurface();
  void DrawLoop();
  void ApplyTranslation(int dx, int dy);
  void ApplyZoom(int new_tx, int new_ty, double new_scale);
  void ApplyBruteForceModification(int tx, int ty, double scale);
  void ApplyModification(const Modification* modification);
  void ProcessSomeFields();

  Board* board_;
  Viewer* viewer_;


  // ----------------------------- Modifications ---------------------------- //

  Modification modification_;
  ObjectUpdaterConsume<Modification> modification_updater_;


  // -------------------------------- Drawing ------------------------------- //

  int width_, height_;

  int tx_, ty_;
  double micro_dx_, micro_dy_;
  double scale_;

  std::set<std::pair<int, int>> fields_to_draw_;

  int current_main_surface_;
  Cairo::RefPtr<Cairo::ImageSurface> main_surface_[2];
  Cairo::RefPtr<Cairo::Context> context_;
  SurfaceBuffer surface_buffers_[3];
  ObjectUpdater<SurfaceBuffer> surface_buffer_updater_;
};

#endif  // PAINTER_H_
