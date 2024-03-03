#pragma once

#include <memory>
#include <optional>
#include "AABB.h"
#include "Interval.h"
#include "pdf/PDF.h"
#include "Ray.h"
#include "vec3.h"

class Material;

struct HitRecord {
  double t;
  point3 p;

  std::shared_ptr<Material> material;
  double u;
  double v;
  vec3 normal;
  bool front_face;
  void set_face_normal(const Ray& ray_in, const vec3& outward_normal) {
    front_face = dot(ray_in.direction, outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};

class Hittable {
 public:
  virtual ~Hittable() = default;
  virtual auto hit(const Ray& ray, Interval ray_interval) const -> std::optional<HitRecord> = 0;
  virtual auto bounding_box() const -> AABB = 0;
};

class ImportantSampled {
 public:
  virtual auto make_important_pdf(const point3& origin) const -> std::shared_ptr<PDF> = 0;
};

class HittableWithPDF : virtual public Hittable, virtual public ImportantSampled {
};

