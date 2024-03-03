#pragma once

#include "Color.h"
#include "Interval.h"
#include "Texture.h"
#include "rtw_stb_image.h"
#include "vec3.h"

class ImageTexture : public Texture {
 public:
  ImageTexture(const char* file_path) : image(file_path) {}

 public:
  auto value(double u, double v, const point3& p) const -> Color override {
    if (image.height() <= 0)
      return Color(0, 0, 0);

    u = Interval(0, 1).clamp(u);
    v = 1.0 - Interval(0, 1).clamp(v);

    auto i = static_cast<int>(u * image.width());
    auto j = static_cast<int>(v * image.height());
    auto pixel = image.pixel_data(i, j);
    auto color_scale = 1.0 / 255.0;

    return Color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
  }

 private:
  rtw_image image;
};



