/**
 * BSD 2-Clause License
 *
 * Copyright (c) 2017, Oscar Asterkrans
 * All rights reserved.
 */

#pragma once
#include "battlesnake.hpp"
#include <cmath>

namespace util {

/** Return the distance between two points. (Manhattan distance)
 */
inline int distance(const Point& a, const Point& b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

}
