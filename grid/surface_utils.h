#ifndef GRID_SURFACE_UTILS_H_
#define GRID_SURFACE_UTILS_H_

#include <cairomm/refptr.h>
#include <cairomm/surface.h>

namespace Grid {

class Options;

void CopySurface(const Cairo::RefPtr<Cairo::ImageSurface>& src,
                 const Cairo::RefPtr<Cairo::ImageSurface>& dst);

void ShiftSurface(const Options& options,
                  const Cairo::RefPtr<Cairo::ImageSurface>& surface,
                  int dx, int dy);

}  // namespace Grid

#endif  // GRID_SURFACE_UTILS_H_
