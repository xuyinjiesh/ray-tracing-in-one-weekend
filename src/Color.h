#pragma once
#include "Interval.h"
#include "vec3.h"

using Color = vec3;

inline double linear_to_gamma(double linear_component) {
    return sqrt(linear_component);
}

auto write_color(std::ostream &out, Color pixel_color, int samples_per_pixel) -> void;