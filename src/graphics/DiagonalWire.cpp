#include "graphics/DiagonalWire.hpp"
#include <cstdlib>

namespace stars {

DiagonalWire::DiagonalWire(int sx, int sy, int ex, int ey, DiagonalType t)
  : sx_(sx), sy_(sy), ex_(ex), ey_(ey), type_(t) {}

bool DiagonalWire::draw(Canvas& canvas) const {
  int dx = (ex_ > sx_) ? 1 : -1;
  int dy = (ey_ > sy_) ? 1 : -1;
  int steps = std::abs(ex_ - sx_);
  char glyph = (type_ == DiagonalType::Slash) ? '\\' : '/';

  int x = sx_, y = sy_;
  for (int i = 1; i <= steps; ++i) {
    x += dx; y += dy;
    if (!canvas.put(x, y, glyph)) return false;
  }
  return true;
}

} // namespace stars
