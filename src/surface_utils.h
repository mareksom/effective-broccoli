#ifndef SURFACE_UTILS_H_
#define SURFACE_UTILS_H_

#include <cairomm/refptr.h>
#include <cairomm/surface.h>

class Options;

void CopySurface(const Cairo::RefPtr<Cairo::ImageSurface>& src,
                 const Cairo::RefPtr<Cairo::ImageSurface>& dst);

void ShiftSurface(const Options& options,
                  const Cairo::RefPtr<Cairo::ImageSurface>& surface,
                  int dx, int dy);

#endif  // SURFACE_UTILS_H_
