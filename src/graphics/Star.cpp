#include "graphics/Star.hpp"

namespace stars {

Star::Star(std::string label) : label_(std::move(label)) {}

int Star::drawWidth() const {
  return 2 + (int)label_.size(); // "* " + label length
}

bool Star::draw(Canvas& canvas, int x, int y) const {
  if (!canvas.put(x, y, '*')) return false;
  if (!canvas.put(x + 1, y, ' ')) return false;
  return canvas.putString(x + 2, y, label_, /*nudgeDown=*/false);
}

} // namespace stars
