#pragma once

#include <memory>
#include "Material.h"
#include "pdf/UniformPDF.h"
#include "texture/SolidColor.h"
#include "texture/Texture.h"

class Isotropic : public Material {
 public:
  Isotropic(const Color& _color) : albedo(std::make_shared<SolidColor>(_color)) {}
  Isotropic(std::shared_ptr<Texture> _texture) : albedo(_texture) {}
  
 public:
  virtual auto emitted(const Ray& ray_in, const HitRecord& hit_record) const -> Color override {
    return Color(0, 0, 0); 
  }
  auto scatter(const Ray& ray_in, const HitRecord& hit_record) const -> std::optional<ScatterRecord> override {
    ScatterRecord scatter_record;
    scatter_record.attenuation = albedo->value(hit_record.u, hit_record.v, hit_record.p);
    scatter_record.mode = ScatterRecord::ScatterMode::WITH_PDF;
    scatter_record.pdf_out = std::make_shared<UniformPDF>();
    return scatter_record;
  }
 private:
  std::shared_ptr<Texture> albedo;
};