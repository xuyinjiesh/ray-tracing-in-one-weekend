#pragma once

#include "PDF.h"

class UniformPDF : public PDF {
 public:
  auto value(const vec3& direction) const -> double override {
    return 1 / (4 * pi);
  }
  auto generate() const -> vec3 override {
    return random_unit_vector();
  }
};
