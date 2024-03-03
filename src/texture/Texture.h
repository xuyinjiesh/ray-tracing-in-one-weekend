#pragma once

#include "Color.h"
#include "vec3.h"

class Texture {
 public:
  virtual ~Texture() = default;
  virtual auto value(double u, double v, const point3& p) const -> Color = 0;
};