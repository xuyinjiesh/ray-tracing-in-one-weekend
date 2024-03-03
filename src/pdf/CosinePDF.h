#pragma once

#include "PDF.h"
#include "ONB.h"

class CosinePDF : public PDF {
 public:
  CosinePDF(const vec3& w) { uvw.build_from_w(w); }
  auto value(const vec3& direction) const -> double override {
    auto cosine_theta = dot(unit_vector(direction), uvw.w());
    return fmax(0, cosine_theta / pi);
  }

  auto generate() const -> vec3 override {
    return uvw.local(random_cosine_direction());
  }

 private:
  vec3 random_cosine_direction() const {
    auto r1 = random_double();
    auto r2 = random_double();

    auto phi = 2 * pi * r1;
    auto x = cos(phi) * sqrt(r2);
    auto y = sin(phi) * sqrt(r2);
    auto z = sqrt(1 - r2);

    return vec3(x, y, z);
  }

 private:
  ONB uvw;
};