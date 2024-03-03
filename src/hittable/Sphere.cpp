#include "Sphere.h"

auto Sphere::make_important_pdf(const point3& origin) const -> std::shared_ptr<PDF> {
  return std::make_shared<SpherePDF>(origin, *this);
}