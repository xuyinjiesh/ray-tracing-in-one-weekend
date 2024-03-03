#pragma once

#include <memory>
#include <optional>
#include "Color.h"
#include "Material.h"
#include "texture/SolidColor.h"
#include "texture/Texture.h"

class DiffuseLight : public Material {
 public:
  DiffuseLight(Color _color) : texture(std::make_shared<SolidColor>(_color)) {}
 public:
  auto emitted(const Ray& ray_in, const HitRecord& hit_record) const -> Color override {
    if (!hit_record.front_face)
      return Color(0, 0, 0);
    return texture->value(hit_record.u, hit_record.v, hit_record.p);
  }
  auto scatter(const Ray& ray_in, const HitRecord& hit_record) const -> std::optional<ScatterRecord> override{
    return std::nullopt;
  }

 private:
  std::shared_ptr<Texture> texture;
};