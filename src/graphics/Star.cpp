#include "graphics/Star.hpp"

using namespace stars;

Star::Star(std::string label) : label_(std::move(label)) {}

int Star::getDrawWidth() const {
    return 2 + (int)label_.size();  // "* " + label length
}

void Star::draw(Canvas& canvas, int x, int y) const {
    canvas.putChar(x, y, '*');
    canvas.putChar(x + 1, y, ' ');
    canvas.putString(x + 2, y, label_, false);
}
