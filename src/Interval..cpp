#include "Interval.h"

const Interval Interval::Empty    = Interval(+infinity, -infinity);
const Interval Interval::Universe = Interval(-infinity, +infinity);

auto operator+(const Interval& interval_old, double offset) -> Interval {
  return Interval(interval_old.min + offset, interval_old.max + offset);
}

auto operator+(double offset, const Interval& interval_old) -> Interval {
  return Interval(interval_old.min + offset, interval_old.max + offset);
}