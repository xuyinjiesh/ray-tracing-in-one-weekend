#pragma once

#include <cstdlib>
#include "Color.h"
#include "Material.h"
#include "vec3.h"

class Metal : public Material {
 public:
  Metal(const Color& _albedo, double _fuzz)
      : albedo(_albedo), fuzz(_fuzz < 1 ? _fuzz : 1) {}
  virtual auto emitted(const Ray& ray_in, const HitRecord& hit_record) const -> Color override {
    return Color(0, 0, 0);  
  }
  virtual auto scatter(const Ray& ray_in, const HitRecord& hit_record) const -> std::optional<ScatterRecord> override {
    ScatterRecord scatter_record;
    scatter_record.attenuation = albedo;
    scatter_record.mode = ScatterRecord::ScatterMode::DIRECT_RAY;
    scatter_record.pdf_out = nullptr;
    auto reflected = reflect(unit_vector(ray_in.direction), hit_record.normal);
    scatter_record.ray_out = Ray(hit_record.p, reflected + fuzz * random_in_unit_sphere());
    return scatter_record;
  }
 private:
  Color albedo;
  double fuzz;
};
