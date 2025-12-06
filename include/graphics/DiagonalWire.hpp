#pragma once
#include "graphics/Wire.hpp"

namespace stars {

enum class DiagonalType { Slash, Backslash };

/// 45° diagonal wire
class DiagonalWire : public Wire {
public:
  DiagonalWire(int sx, int sy, int ex, int ey, DiagonalType t);
  bool draw(Canvas& canvas) const override;

private:
  int sx_, sy_, ex_, ey_;
  DiagonalType type_;
};

} // namespace stars
