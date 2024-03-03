#pragma once

#include <memory>
#include <optional>
#include "Color.h"
#include "Hittable.h"
#include "Interval.h"
#include "material/Isotropic.h"
#include "material/Material.h"
#include "texture/Texture.h"

class ConstantMedium : public Hittable {
 public:
  ConstantMedium(std::shared_ptr<Hittable> _boundary, double _density, Color _color)
      : boundary(_boundary), neg_inv_density(-1 / _density), phase_function(std::make_shared<Isotropic>(_color)) {}
  ConstantMedium(std::shared_ptr<Hittable> _boundary, double _density, std::shared_ptr<Texture> _texture)
      : boundary(_boundary), neg_inv_density(-1 / _density), phase_function(std::make_shared<Isotropic>(_texture)) {}

 public:
  virtual auto hit(const Ray& ray, Interval ray_interval) const -> std::optional<HitRecord> override {
        // Print occasional samples when debugging. To enable, set enableDebug true.
        const bool enableDebug = false;
        const bool debugging = enableDebug && random_double() < 0.00001;

        auto rec1 = boundary->hit(ray, Interval::Universe);
        if (rec1 == std::nullopt)
          return std::nullopt;
        auto rec2 = boundary->hit(ray, Interval(rec1->t + 0.0001, infinity));
        if (rec2 == std::nullopt)
          return std::nullopt;
        
        if (debugging) std::clog << "\nt_min=" << rec1->t << ", t_max=" << rec2->t << '\n';

        if (rec1->t < ray_interval.min) rec1->t = ray_interval.min;
        if (rec2->t > ray_interval.max) rec2->t = ray_interval.max;

        if (rec1->t >= rec2->t)
            return std::nullopt;

        if (rec1->t < 0)
            rec1->t = 0;

        auto ray_length = ray.direction.length();
        auto distance_inside_boundary = (rec2->t - rec1->t) * ray_length;
        auto hit_distance = neg_inv_density * log(random_double());

        if (hit_distance > distance_inside_boundary)
            return std::nullopt;

        HitRecord hit_record;
        hit_record.t = rec1->t + hit_distance / ray_length;
        hit_record.p = ray.at(hit_record.t);

        if (debugging) {
            std::clog << "hit_distance = " <<  hit_distance << '\n'
                      << "rec.t = " <<  hit_record.t << '\n'
                      << "rec.p = " <<  hit_record.p << '\n';
        }

        hit_record.normal = vec3(1,0,0);  // arbitrary
        hit_record.front_face = true;     // also arbitrary
        hit_record.material = phase_function;

        return hit_record;
  }
  virtual auto bounding_box() const -> AABB override {
    return boundary->bounding_box();      
  }

 private:
  std::shared_ptr<Hittable> boundary;
  double neg_inv_density;
  std::shared_ptr<Material> phase_function;
};