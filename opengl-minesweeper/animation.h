#pragma once

#include <algorithm>

using namespace std;

namespace Animation {
  inline double bezier(double t, const double p1, const double p2, const double p3, const double p4) {
    t = min(1.0, max(0.0, t));
    return pow(1 - t, 3) * p1 + 3 * pow(1 - t, 2) * t * p2 + 3 * (1 - t) * pow(t, 2) * p3 + pow(t, 3) * p4;
  }
  inline double linear(double t) {
    return bezier(t, 0, 0, 1, 1);
  }
  inline double ease(double t) {
    return bezier(t, 0.25, 0.1, 0.25, 1);
  }
  inline double easeIn(double t) {
    return bezier(t, 0.42, 0, 1, 1);
  }
  inline double easeOut(double t) {
    return bezier(t, 0, 0, 0.58, 1);
  }
  inline double easeInOut(double t) {
    return bezier(t, 0.42, 0, 0.58, 1);
  }
}  // namespace Animation
