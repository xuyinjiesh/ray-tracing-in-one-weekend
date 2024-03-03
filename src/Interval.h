#pragma once
#include <cmath>
#include <limits>
#include "vec3.h"

const double infinity = std::numeric_limits<double>::infinity();

struct Interval {

  double min, max;
  Interval()
      : min(+infinity), max(-infinity) {}
  Interval(double _min, double _max)
      : min(_min), max(_max) {}
  Interval(const Interval& i1, const Interval i2)
      : min(std::fmin(i1.min, i2.min)),
        max(std::fmax(i1.max, i2.max)) {}

  auto clamp(double x) const -> double {
    if (x < min) return min;
    if (x > max) return max;
    return x;
  }

  auto contains(double x) const -> bool {
    return min <= x && x <= max;
  }

  auto expand(double delta) const -> Interval {
    double padding = delta / 2;
    return Interval(min - padding, max + padding);
  }

  auto size() const -> double {
    return max - min;
  }
  
  auto surrounds(double x) const -> bool {
    return min < x && x < max;
  }

 public:
  static const Interval Empty;
  static const Interval Universe;
};


auto operator+(const Interval& interval_old, double offset) -> Interval;
auto operator+(double offset, const Interval& interval_old) -> Interval;

