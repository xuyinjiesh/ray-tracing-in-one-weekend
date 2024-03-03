#pragma once
#include "vec3.h"

struct Ray {
 public:
  point3 origin;
  vec3 direction;

 public:
  Ray(const point3& _origin, const vec3& _direction)
      : origin(_origin), direction(_direction) {}

 public:
  auto at(double t) const -> point3 { return origin + t * direction; }
};

struct RayWithTime : public Ray {


};