#include "AABB.h"

auto operator+(const AABB& aabb, const vec3& offset) -> AABB {
  return AABB(aabb.ix + offset.x(), aabb.iy + offset.y(), aabb.iz + offset.z());
}

auto operator+(const vec3& offset, const AABB& aabb) -> AABB {
  return AABB(aabb.ix + offset.x(), aabb.iy + offset.y(), aabb.iz + offset.z());
}