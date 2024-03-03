#pragma once

#include <optional>
#include "Color.h"
#include "Material.h"

class Dielectric : public Material {
 public:
  Dielectric(double _index_of_refraction) : ir(_index_of_refraction) {}

 public:
  virtual auto emitted(const Ray& ray_in, const HitRecord& hit_record) const -> Color override {
    return Color(0, 0, 0);  
  }
  virtual auto scatter(const Ray& ray_in, const HitRecord& hit_record) const -> std::optional<ScatterRecord> override {
    ScatterRecord scatter_record;
    scatter_record.attenuation = Color(1.0, 1.0, 1.0);
    scatter_record.mode = ScatterRecord::ScatterMode::DIRECT_RAY;
    scatter_record.pdf_out = nullptr;

    vec3 unit_direction = unit_vector(ray_in.direction);
    double cos_theta = fmin(dot(-unit_direction, hit_record.normal), 1.0);
    double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

    double refraction_ratio = hit_record.front_face ? (1.0 / ir) : ir;
    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
    vec3 direction;

    if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
      direction = reflect(unit_direction, hit_record.normal);
    else
      direction = refract(unit_direction, hit_record.normal, refraction_ratio);

    scatter_record.ray_out = Ray(hit_record.p, direction);
    return scatter_record;
  }
  static double reflectance(double cosine, double ref_idx) {
    // Use Schlick's approximation for reflectance.
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
  }

 private:
  double ir; // Index of Refraction
};
