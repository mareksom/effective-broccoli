#include "surface_utils.h"

#include <cassert>
#include <cstring>

#include "options.h"

void CopySurface(const Cairo::RefPtr<Cairo::ImageSurface>& src,
                 const Cairo::RefPtr<Cairo::ImageSurface>& dst) {
  const int height = src->get_height();
  const int stride = src->get_stride();
  assert(height == dst->get_height());
  assert(stride == dst->get_stride());
  src->flush();
  dst->flush();
  unsigned char* src_data = src->get_data();
  unsigned char* dst_data = dst->get_data();
  for (int y = 0; y < height; y++) {
    std::memcpy(dst_data + y * stride, src_data + y * stride, stride);
  }
  dst->mark_dirty();
}

void ShiftSurface(
    const Cairo::RefPtr<Cairo::ImageSurface>& surface, int dx, int dy) {
  const int width = surface->get_width();
  const int height = surface->get_height();
  const int stride = surface->get_stride();
  auto format = surface->get_format();
  assert(format == Cairo::Format::FORMAT_RGB24 or
         format == Cairo::Format::FORMAT_ARGB32);
  assert(stride >= width * 4);
  surface->flush();
  const int null_color = options.NullColor();
  assert(0 <= null_color and null_color < 256);
  unsigned char* data = surface->get_data();
  if (width == 0 or height == 0) {
    return;
  } else if (dx == 0 and dy == 0) {
    return;
  } else {
    if (dy < 0) {
      for (int y = -dy; y < height; y++) {
        std::memcpy(data + (y + dy) * stride, data + y * stride, width * 4);
      }
      for (int y = height + dy; y < height; y++) {
        std::memset(data + y * stride, null_color, width * 4);
      }
    } else if (dy > 0) {
      for (int y = height - 1 - dy; y >= 0; y--) {
        std::memcpy(data + (y + dy) * stride, data + y * stride, width * 4);
      }
      for (int y = dy - 1; y >= 0; y--) {
        std::memset(data + y * stride, null_color, width * 4);
      }
    }
    if (dx < 0) {
      for (int y = 0; y < height; y++) {
        std::memmove(data + y * stride, data + (y * stride - dx * 4),
                     (width + dx) * 4);
        std::memset(data + (y * stride + (width + dx) * 4),
                    null_color, -dx * 4);
      }
    } else if (dx > 0) {
      for (int y = 0; y < height; y++) {
        std::memmove(data + (y * stride + dx * 4), data + y * stride,
                     (width - dx) * 4);
        std::memset(data  + y * stride, null_color, dx * 4);
      }
    }
  }
  surface->mark_dirty();
}
