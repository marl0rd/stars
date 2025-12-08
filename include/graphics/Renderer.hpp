#pragma once
#include <string>
#include <vector>

#include "graphics/Canvas.hpp"
#include "graphics/DiagonalWire.hpp"
#include "graphics/HorizontalWire.hpp"
#include "graphics/Star.hpp"

namespace stars {

struct Ray {
    std::string label;          ///< Example: "<ls -l>"
    int len;                    ///< branch length (score), clamped [2, 20]
};

/// Render one constellation: base on same row, rays to the left of center (aligned like sample).
class Renderer {
   public:
    struct Parameters {
        int canvasWidth = 100;
        int canvasHeight = 28;
        int centerX = 60;
        int centerY = 12;
        int labelGap = 3;
    };

    static std::string getConstellation(const std::string& baseLabel,
                                        const std::string& centerLabel,
                                        const std::vector<Ray>& upRays,
                                        const std::vector<Ray>& downRays,
                                        const Parameters& params);

   private:
    static int rightEdgeX(const Star& s, int sx);
    static void placeLeftLabelThenWire(Canvas& canvas,
                                       const Star& s,
                                       int wireStartX, int wireStartY,
                                       int gap, int centerX, int centerY);
};

}  // namespace stars
