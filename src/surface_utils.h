#ifndef SURFACE_UTILS_H_
#define SURFACE_UTILS_H_

#include <cairomm/refptr.h>
#include <cairomm/surface.h>

void ShiftSurface(
    const Cairo::RefPtr<Cairo::ImageSurface>& surface, int dx, int dy);

void CopySurface(const Cairo::RefPtr<Cairo::ImageSurface>& src,
                 const Cairo::RefPtr<Cairo::ImageSurface>& dst);

#endif  // SURFACE_UTILS_H_
