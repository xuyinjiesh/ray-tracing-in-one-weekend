#pragma once

#include <cstdlib>
#include <memory>
#include "vec3.h"

class PDF {
 public:
  virtual ~PDF() = default;
 public:
  virtual auto value(const vec3& direction) const -> double = 0;
  virtual auto generate() const -> vec3 = 0;
};
