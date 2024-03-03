#pragma once
#include "Color.h"
#include "Texture.h"

class SolidColor : public Texture {
 public:
  SolidColor(Color _color) : color(_color) {}
  SolidColor(double red, double green, double blue) : SolidColor(Color(red, green, blue)) {}

 public:
  auto value(double u, double v, const point3& p) const -> Color override { return color; }

 private:
  Color color;
};