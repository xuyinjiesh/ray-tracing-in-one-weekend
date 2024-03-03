#pragma once
#include <memory>
#include <optional>
#include "Color.h"
#include "hittable/Hittable.h"
#include "pdf/PDF.h"
#include "Ray.h"
#include "texture/Texture.h"
#include "vec3.h"

struct ScatterRecord {
  Color attenuation;
  enum class ScatterMode {DIRECT_RAY, WITH_PDF} mode;
  std::optional<Ray> ray_out;
  std::shared_ptr<PDF> pdf_out;
};

class Material {
 public:
  virtual ~Material() = default;
  virtual auto emitted(const Ray& ray_in, const HitRecord& hit_record) const -> Color = 0;
  virtual auto scatter(const Ray& ray_in, const HitRecord& hit_record) const -> std::optional<ScatterRecord> = 0;
};


