#include "graphics/HorizontalWire.hpp"
#include <algorithm>

namespace stars {

HorizontalWire::HorizontalWire(int x1, int x2, int y)
  : x1_(x1), x2_(x2), y_(y) {}

bool HorizontalWire::draw(Canvas& canvas) const {
  int left = std::min(x1_, x2_);
  int right = std::max(x1_, x2_);
  for (int x = left + 1; x < right; ++x) {
    if (!canvas.put(x, y_, '-')) return false;
  }
  return true;
}

} // namespace stars
