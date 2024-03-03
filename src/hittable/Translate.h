#pragma once

#include <cassert>
#include <memory>
#include "AABB.h"
#include "Hittable.h"
#include "Ray.h"
#include "vec3.h"

class Translate : public Hittable {
 public:
  Translate(std::shared_ptr<Hittable> _object, const vec3& _offset)
      : object(_object), offset(_offset), aabb(object->bounding_box() + _offset) {}
  virtual auto hit(const Ray& ray, Interval ray_interval) const -> std::optional<HitRecord> override {
    Ray ray_new(ray.origin - offset, ray.direction);
    auto hit_record = object->hit(ray_new, ray_interval);
    if (hit_record.has_value())
      hit_record->p += offset;
    return hit_record;
  }
  virtual auto bounding_box() const -> AABB override {
    return aabb;
  }
 private:
  std::shared_ptr<Hittable> object;
  vec3 offset;
  AABB aabb;
};




