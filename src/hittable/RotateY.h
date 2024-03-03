#pragma once

#include <cassert>
#include <cstddef>
#include <memory>
#include <optional>
#include "AABB.h"
#include "Hittable.h"

class RotateY : public Hittable {
 public:
  RotateY(std::shared_ptr<Hittable> _object, double angle)
      : object(_object) {
    auto radians = degrees_to_radians(angle);
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    aabb = object->bounding_box();

    point3 min(infinity, infinity, infinity);
    point3 max(-infinity, -infinity, -infinity);

    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 2; j++) {
        for (int k = 0; k < 2; k++) {
          auto x = i * aabb.ix.max + (1 - i) * aabb.ix.min;
          auto y = j * aabb.iy.max + (1 - j) * aabb.iy.min;
          auto z = k * aabb.iz.max + (1 - k) * aabb.iz.min;

          auto newx = cos_theta * x + sin_theta * z;
          auto newz = -sin_theta * x + cos_theta * z;

          vec3 tester(newx, y, newz);

          for (int c = 0; c < 3; c++) {
            min[c] = fmin(min[c], tester[c]);
            max[c] = fmax(max[c], tester[c]);
          }
        }
      }
    }

    aabb = AABB(min, max);
  }

 public:
  virtual auto hit(const Ray& ray, Interval ray_interval) const -> std::optional<HitRecord> override {
    // Change the ray from world space to object space
    auto origin = ray.origin;
    auto direction = ray.direction;

    origin[0] = cos_theta * ray.origin[0] - sin_theta * ray.origin[2];
    origin[2] = sin_theta * ray.origin[0] + cos_theta * ray.origin[2];

    direction[0] = cos_theta * ray.direction[0] - sin_theta * ray.direction[2];
    direction[2] = sin_theta * ray.direction[0] + cos_theta * ray.direction[2];

    Ray rotated_r(origin, direction);

    // Determine whether an intersection exists in object space (and if so, where)
    auto hit_record = object->hit(rotated_r, ray_interval);
    if (!hit_record.has_value())
      return std::nullopt;

    // Change the intersection point from object space to world space
    auto p = hit_record->p;
    p[0] = cos_theta * hit_record->p[0] + sin_theta * hit_record->p[2];
    p[2] = -sin_theta * hit_record->p[0] + cos_theta * hit_record->p[2];

    // Change the normal from object space to world space
    auto normal = hit_record->normal;
    normal[0] = cos_theta * hit_record->normal[0] + sin_theta * hit_record->normal[2];
    normal[2] = -sin_theta * hit_record->normal[0] + cos_theta * hit_record->normal[2];

    hit_record->p = p;
    hit_record->normal = normal;

    return hit_record;
  }
  virtual auto bounding_box() const -> AABB override {
    return aabb;
  }

 private:
  std::shared_ptr<Hittable> object;
  double sin_theta;
  double cos_theta;
  AABB aabb;
};

