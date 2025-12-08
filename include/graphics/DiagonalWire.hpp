#pragma once
#include <cstdlib>
#include <stdexcept>

#include "graphics/Canvas.hpp"

namespace stars {

class DiagonalWire {
   public:
    DiagonalWire(int startX, int startY, int endX, int endY);

    /// Draw the diagonal (45°) wire from (startX, startY) to (endX, endY).
    /// Precondition: |deltaX| == |deltaY| and both != 0 (strict 45° line).
    void draw(Canvas& canvas) const;

   private:
    int startX_;
    int startY_;
    int endX_;
    int endY_;

    static inline int signOf(int value) {
        return (value > 0) - (value < 0);
    }

    static inline bool isFortyFiveDegrees(int deltaX, int deltaY) {
        return deltaX != 0 && deltaY != 0 && std::abs(deltaX) == std::abs(deltaY);
    }

    static inline bool hasPositiveSlope(int deltaX, int deltaY) {
        return deltaX * deltaY > 0;  // Same sign => positive slope
    }

    static char getGlyph(int deltaX, int deltaY);
};

}  // namespace stars