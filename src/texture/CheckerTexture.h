#pragma once

#include <memory>
#include "Color.h"
#include "Texture.h"
#include "texture/SolidColor.h"

class CheckerTexture : public Texture {
 public:
  CheckerTexture(double _scale, std::shared_ptr<Texture> _even, std::shared_ptr<Texture> _odd)
      : inv_scale(1.0 / _scale), even(_even), odd(_odd) {}
  CheckerTexture(double _scale, Color _color1, Color _color2)
      : inv_scale(1.0 / _scale), even(std::make_shared<SolidColor>(_color1)), odd(std::make_shared<SolidColor>(_color2)) {}

 public:
  virtual auto value(double u, double v, const point3& p) const -> Color override {
    auto xInteger = static_cast<int>(std::floor(inv_scale * p.x()));
    auto yInteger = static_cast<int>(std::floor(inv_scale * p.y()));
    auto zInteger = static_cast<int>(std::floor(inv_scale * p.z()));

    bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

    return isEven ? even->value(u, v, p) : odd->value(u, v, p);
  }

 private:
  double inv_scale;
  std::shared_ptr<Texture> even;
  std::shared_ptr<Texture> odd;
};