#pragma once
#include "offsets.h"

inline float distTo(const Vector& a, const Vector& b) {
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}