#pragma once

#include <iostream>
#include "PDF.h"

class MixturePDF : public PDF {
 public:
  MixturePDF(std::shared_ptr<PDF> _p1, std::shared_ptr<PDF> _p2)
      : p1(_p1), p2(_p2) {}
 public:
  auto value(const vec3& direction) const -> double override {
    return 0.5 * p1->value(direction) + 0.5 * p2->value(direction);
  }
  auto generate() const -> vec3 override {
    if (random_double() < 0.5) {
      // std::cout << p1->generate() << '\n';
      return p1->generate();
    } else {
      // std::cout << p2->generate() << '\n';  
      return p2->generate();
    }
  }

 private:
  std::shared_ptr<PDF> p1;
  std::shared_ptr<PDF> p2;
};