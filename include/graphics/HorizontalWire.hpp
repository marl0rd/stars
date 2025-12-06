#pragma once
#include "graphics/Wire.hpp"

namespace stars {

/// Horizontal wire '-' from (x1, y) to (x2, y)
class HorizontalWire : public Wire {
public:
  HorizontalWire(int x1, int x2, int y);
  bool draw(Canvas& canvas) const override;

private:
  int x1_, x2_, y_;
};

} // namespace stars
