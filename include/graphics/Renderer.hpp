#pragma once
#include <string>
#include <vector>

#include "graphics/Canvas.hpp"
#include "graphics/DiagonalWire.hpp"
#include "graphics/HorizontalWire.hpp"
#include "graphics/Star.hpp"

namespace stars {

class Renderer {
   public:
    struct Ray {
        std::string label;  ///< Example: "<ls -l>"
        int len;            ///< Branch length clamped [2, 10]
    };

    struct Parameters {
        int canvasWidth = 100;
        int canvasHeight = 28;
        int centerX = 60;
        int centerY = 12;
        int labelGap = 0;
    };

    /// Draw a single constellation and return the ASCII canvas as a string.
    static std::string getConstellation(const std::string& baseLabel,
                                        const std::string& centerLabel,
                                        const std::vector<Ray>& upRays,
                                        const std::vector<Ray>& downRays,
                                        const Parameters& params);

   private:
    static int rightEdgeX(const Star& s, int startX);

    static void placeLeftLabelThenWire(Canvas& canvas,
                                       const Star& labelStar,
                                       int wireStartX, int wireStartY,
                                       int gap, int centerX, int centerY);

    static int clampRayLength(int len);

    static inline std::pair<int, int> startForUpRay(int centerX, int centerY, int length) {
        return {centerX - length, centerY - length};
    }

    static inline std::pair<int, int> startForDownRay(int centerX, int centerY, int length) {
        return {centerX - length, centerY + length};
    }

    static void drawBaseAndConnector(Canvas& canvas,
                                     const std::string& baseLabel,
                                     int centerX, int centerY,
                                     int desiredWireColumns);

    static void drawRay(Canvas& canvas,
                        const Ray& ray,
                        bool isUpward,
                        const Parameters& params);
};

}  // namespace stars
