#pragma once
#include <string>
#include <vector>

#include "graphics/Canvas.hpp"
#include "graphics/DiagonalWire.hpp"
#include "graphics/HorizontalWire.hpp"
#include "graphics/Star.hpp"

namespace stars {

// TODO: We need to do this automatically later
struct RenderParams {
    int canvasWidth = 100;
    int canvasHeight = 28;
    int centerX = 60;
    int centerY = 12;
    int labelGap = 3;
};

struct Ray {
    std::string label;  ///< Example: "<ls -l>"
    int len;            ///< branch length (score), clamped [2, 20]
    DiagonalType type;  ///< Slash or Backslash
};

/// Render one constellation: base on same row, rays to the left of center (aligned like sample).
class Renderer {
   public:
    static std::string getRenderedConstellation(const std::string& baseLabel,
                                           const std::string& centerLabel,
                                           const std::vector<Ray>& upRays,
                                           const std::vector<Ray>& downRays,
                                           const RenderParams& p);

   private:
    static int rightEdgeX(const Star& s, int sx);
    static void placeLeftLabelThenWire(Canvas& canvas,
                                       const Star& s,
                                       int wireStartX, int wireStartY,
                                       DiagonalType type,
                                       int gap, int centerX, int centerY);
};

}  // namespace stars
