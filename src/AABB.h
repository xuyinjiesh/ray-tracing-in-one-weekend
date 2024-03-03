#pragma once
#include "Interval.h"
#include "Ray.h"
#include "vec3.h"

struct AABB {
 public:
  Interval ix, iy, iz;

 public:
  AABB() = default;
  AABB(const Interval& _ix, const Interval& _iy, const Interval _iz)
      : ix(_ix), iy(_iy), iz(_iz) {}
  AABB(const point3& p1, const point3& p2)
      : ix(fmin(p1[0], p2[0]), fmax(p1[0], p2[0])),
        iy(fmin(p1[1], p2[1]), fmax(p1[1], p2[1])),
        iz(fmin(p1[2], p2[2]), fmax(p1[2], p2[2])) {}
  AABB(const AABB& aabb1, const AABB& aabb2)
      : ix(aabb1.ix, aabb2.ix),
        iy(aabb1.iy, aabb2.iy),
        iz(aabb1.iz, aabb2.iz) {}

 public:
  const Interval& axis(int n) const {
    if (n == 1) return iy;
    if (n == 2) return iz;
    return ix;
  }
  AABB pad() {
    double delta = 0.001;
    Interval new_ix = (ix.size() >= delta) ? ix : ix.expand(delta);
    Interval new_iy = (iy.size() >= delta) ? iy : iy.expand(delta);
    Interval new_iz = (iz.size() >= delta) ? iz : iz.expand(delta);
    return AABB(new_ix, new_iy, new_iz);
  }
  auto hit(const Ray& r, Interval ray_t) const -> bool {
    for (int a = 0; a < 3; a++) {
      auto invD = 1 / r.direction[a];
      auto orig = r.origin[a];

      auto t0 = (axis(a).min - orig) * invD;
      auto t1 = (axis(a).max - orig) * invD;

      if (invD < 0)
        std::swap(t0, t1);

      if (t0 > ray_t.min) ray_t.min = t0;
      if (t1 < ray_t.max) ray_t.max = t1;

      if (ray_t.max <= ray_t.min)
        return false;
    }
    return true;
  }
};

auto operator+(const AABB& aabb, const vec3& offset) -> AABB;
auto operator+(const vec3& offset, const AABB& aabb) -> AABB;