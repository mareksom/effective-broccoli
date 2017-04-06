#ifndef GRID_PAINTER_H_
#define GRID_PAINTER_H_

#include <cairomm/context.h>
#include <cairomm/surface.h>
#include <mutex>
#include <set>
#include <utility>

#include "lock_free_queue.h"
#include "object_updater.h"

namespace Grid {

class Board;
class Options;
class Viewer;

class Painter {
 public:
  struct SurfaceBuffer {
    Cairo::RefPtr<Cairo::ImageSurface> surface;
    double start_x;
    double start_y;
  };

  Painter(const Options* options, Board* board, int width, int height);

  void SetViewer(Viewer* viewer);
  void Start();

  const SurfaceBuffer* GetAndLockCurrentSurfaceBuffer();
  void ReleaseCurrentSurfaceBuffer();

  void Resize(int width, int height);
  void Translate(double dx, double dy);
  void Zoom(double x, double y, double factor);

  void InvalidateField(int x, int y);
  void CenterOn(int x, int y);

  std::pair<int, int> WindowToBoardCoordinates(double x, double y) const;

 private:
  const Options& options() const;

  struct Modification {
    double tx;
    double ty;
    double scale;
    int width;
    int height;
  };

  std::pair<double, double> BoardToSurfaceCoordinates(double x, double y) const;
  std::pair<double, double> SurfaceToBoardCoordinates(double x, double y) const;

  // @SetModification() requires @update_mutex_ being locked.
  void SetModification();
  void UpdateCurrentSurface();
  void DrawLoop();
  void ApplyTranslation(int dx, int dy);
  void ApplyZoom(int new_tx, int new_ty, double new_scale);
  void ApplyBruteForceModification(int tx, int ty, double scale);
  void ApplyModification(const Modification* modification);
  void ProcessSomeFields();

  const Options* options_;
  Board* board_;
  Viewer* viewer_;


  // --------------------------------- Tasks -------------------------------- //

  std::mutex update_mutex_;
  Modification modification_;
  LockFreeQueue<std::function<void()>, 50> task_queue_;


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

}  // GRID_namespace Grid

#endif  // PAINTER_H_
