#pragma once

#include <math.h>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <utility>
#include "AABB.h"
#include "Hittable.h"
#include "Interval.h"
#include "ONB.h"
#include "Ray.h"
#include "vec3.h"

class Sphere : public HittableWithPDF {
  class SpherePDF;
 public:
  Sphere(const point3& _center, double _radius, std::shared_ptr<Material> _material)
      : center(_center), radius(_radius), material(_material),
        aabb([_center, _radius]() -> AABB {
          auto rvec = vec3(_radius, _radius, _radius);
          return AABB(_center - rvec, _center + rvec);
        }()) {}

 public:
  virtual auto hit(const Ray& ray, Interval ray_interval) const -> std::optional<HitRecord> override {
    vec3 oc = ray.origin - center;
    auto a = ray.direction.length_squared();
    auto half_b = dot(oc, ray.direction);
    auto c = oc.length_squared() - radius * radius;

    auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0)
      return std::nullopt;

    // Find the nearest root that lies in the acceptable range.
    auto sqrtd = sqrt(discriminant);
    auto root = (-half_b - sqrtd) / a;
    if (!ray_interval.surrounds(root)) {
      root = (-half_b + sqrtd) / a;
      if (!ray_interval.surrounds(root))
        return std::nullopt;
    }

    HitRecord hit_record;
    hit_record.t = root;
    hit_record.p = ray.at(hit_record.t);
    vec3 outward_normal = (hit_record.p - center) / radius;
    hit_record.set_face_normal(ray, outward_normal);

    static auto get_sphere_uv = [](const point3& p) -> std::pair<double, double> {
      auto theta = acos(-p.y());
      auto phi = atan2(-p.z(), p.x()) + pi;
      return std::make_pair(phi / (2 * pi), theta / pi);
    };
    auto [u, v] = get_sphere_uv(outward_normal);
    hit_record.u = u;
    hit_record.v = v;

    hit_record.material = material;

    return hit_record;
  }
  virtual auto bounding_box() const -> AABB override {
    return aabb;
  }
  virtual auto make_important_pdf(const point3& origin) const -> std::shared_ptr<PDF> override;
 private:
  point3 center;
  double radius;
  std::shared_ptr<Material> material;
  AABB aabb;
};

class Sphere::SpherePDF : public PDF {
 public:
  SpherePDF(const vec3& _origin, const Sphere& _sphere)
      : origin(_origin), sphere(_sphere) {}
  virtual auto value(const vec3& direction) const -> double override {
    auto hit_record = sphere.hit(Ray(origin, direction), Interval(0.001, infinity));
    if (!hit_record.has_value())
      return 0.0;
    auto cos_theta_max = sqrt(1 - sphere.radius * sphere.radius / (sphere.center - origin).length_squared());
    auto solid_angle = 2 * pi * (1 - cos_theta_max);

    return 1 / solid_angle;
  }
  virtual auto generate() const -> vec3 override {
    vec3 direction = sphere.center - origin;
    auto distance_squared = direction.length_squared();
    ONB uvw;
    uvw.build_from_w(direction);
    static auto random_to_sphere = [](double radius, double distance_squared) {
      auto r1 = random_double();
      auto r2 = random_double();
      auto z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);
      auto phi = 2 * pi * r1;
      auto x = cos(phi) * sqrt(1 - z * z);
      auto y = sin(phi) * sqrt(1 - z * z);
      return vec3(x, y, z);
    };
    return uvw.local(random_to_sphere(sphere.radius, distance_squared));
  }
 private:
  const vec3& origin;
  const Sphere& sphere;
};
