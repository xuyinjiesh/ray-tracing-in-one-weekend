#pragma once

#include <memory>
#include "AABB.h"
#include "Hittable.h"
#include "hittable/HittableList.h"
#include "vec3.h"

class Quadrilateral : public HittableWithPDF {
  class QuadrilateralPDF;
  
 public:
  Quadrilateral(const point3& _Q, const vec3& _u, const vec3& _v, std::shared_ptr<Material> _material);

 public:
  auto hit(const Ray& ray, Interval ray_interval) const -> std::optional<HitRecord> override;
  auto bounding_box() const -> AABB override;
  auto make_important_pdf(const point3& origin) const -> std::shared_ptr<PDF> override;
 private:
  point3 Q;
  vec3 u, v;
  std::shared_ptr<Material> material;

 private:
  struct Cache {
    vec3 n;
    double D;
    double area;
    vec3 w;
  } cache;
};

class Quadrilateral::QuadrilateralPDF : public PDF {
 public:
  QuadrilateralPDF(const point3& _origin, const Quadrilateral& _quadrilateral)
      : origin(_origin), quadrilateral(_quadrilateral) {}
  auto value(const vec3& direction) const -> double override {
    auto hit_record = quadrilateral.hit(Ray(origin, direction), Interval(0.001, infinity));
    if (!hit_record.has_value())
      return 0.0;
    auto distance_squared = hit_record->t * hit_record->t * direction.length_squared();
    auto cosine = fabs(dot(direction, hit_record->normal) / direction.length());
    return distance_squared / (cosine * quadrilateral.cache.area);
  }
  auto generate() const -> vec3 override {
    auto p = quadrilateral.Q + (random_double() * quadrilateral.u) + (random_double() * quadrilateral.v);
    return unit_vector(p - origin);
  }

 private:
  const vec3& origin;
  const Quadrilateral& quadrilateral;
};

auto make_box(const point3& a, const point3& b, std::shared_ptr<Material> material) -> std::shared_ptr<HittableListWithPDF>;

