#include <memory>
#include "HittableList.h"
#include "pdf/UniformPDF.h"

auto HittableListWithPDF::make_important_pdf(const point3& origin) const -> std::shared_ptr<PDF> {
  if (objects.size() == 0)
    return std::make_shared<UniformPDF>();  
  return std::make_shared<HittableListPDF>(origin, *this);
}
