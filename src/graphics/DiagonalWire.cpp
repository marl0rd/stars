#include "graphics/DiagonalWire.hpp"

using namespace stars;

DiagonalWire::DiagonalWire(int startX, int startY, int endX, int endY)
    : startX_(startX), startY_(startY), endX_(endX), endY_(endY) {}

void DiagonalWire::draw(Canvas& canvas) const {
    const int deltaX = endX_ - startX_;
    const int deltaY = endY_ - startY_;

    if (!isFortyFiveDegrees(deltaX, deltaY)) {
        throw std::invalid_argument(
            "DiagonalWire::draw requires a strict 45-degree segment (|deltaX| == |deltaY| != 0)");
    }

    const int stepX = signOf(deltaX);     // +1 (right) or -1 (left)
    const int stepY = signOf(deltaY);     // +1 (down) or -1 (up)
    const int length = std::abs(deltaX);  // equals std::abs(deltaY) here

    const char glyph = getGlyph(deltaX, deltaY);

    int x = startX_;
    int y = startY_;

    // Walk exactly 'length' steps to reach (endX_, endY_)
    for (int i = 0; i < length; ++i) {
        x += stepX;
        y += stepY;
        canvas.putChar(x, y, glyph);
    }
}

char DiagonalWire::getGlyph(int deltaX, int deltaY) {
    // Decide from slope sign:
    // same sign => positive slope => backslash
    // opposite sign => negative slope => slash
    return hasPositiveSlope(deltaX, deltaY) ? '\\' : '/';
}