#include "graphics/Renderer.hpp"

#include <algorithm>
#include <iostream>

using namespace stars;

static constexpr int kMinRayLen = 2;
static constexpr int kMaxRayLen = 10;
static constexpr int kDesiredWire = 10;

int Renderer::rightEdgeX(const Star& s, int startX) {
    return startX + s.getDrawWidth() - 1;
}

int Renderer::clampRayLength(int len) {
    if (len < kMinRayLen) return kMinRayLen;
    if (len > kMaxRayLen) return kMaxRayLen;
    return len;
}

void Renderer::placeLeftLabelThenWire(Canvas& canvas,
                                      const Star& labelStar,
                                      int wireStartX, int wireStartY,
                                      int gap, int centerX, int centerY) {
    // Place label so its right edge sits 'gap' columns before the wire start.
    int labelStartX = wireStartX - gap - labelStar.getDrawWidth();
    if (labelStartX < 0) labelStartX = 0;  // keep inside canvas
    const int labelStartY = wireStartY;

    // Draw the label star at the computed position.
    labelStar.draw(canvas, labelStartX, labelStartY);

    // Draw diagonal wire from start to center (glyph auto-chosen by DiagonalWire).
    DiagonalWire wire(wireStartX, wireStartY, centerX, centerY);
    wire.draw(canvas);
}

void Renderer::drawBaseAndConnector(Canvas& canvas,
                                    const std::string& baseLabel,
                                    int centerX, int centerY,
                                    int desiredWireColumns) {
    Star base(baseLabel);

    // Compute a left position that leaves 'desiredWireColumns' between base and center.
    int baseStartX = centerX - desiredWireColumns - base.getDrawWidth();
    if (baseStartX < 0) baseStartX = 0;

    // Draw the base star.
    base.draw(canvas, baseStartX, centerY);

    // Connect base to center on the same row with a horizontal wire.
    const int baseRightX = rightEdgeX(base, baseStartX);
    HorizontalWire hwire(baseRightX, centerX, centerY);
    hwire.draw(canvas);
}

void Renderer::drawRay(Canvas& canvas,
                       const Ray& ray,
                       bool isUpward,
                       const Parameters& params) {
    // Clamp length and compute wire start.
    const int length = clampRayLength(ray.len);
    const auto start = isUpward
                           ? startForUpRay(params.centerX, params.centerY, length)
                           : startForDownRay(params.centerX, params.centerY, length);

    // Build label star and draw label + wire.
    Star labelStar(ray.label);
    try {
        placeLeftLabelThenWire(canvas, labelStar, start.first, start.second,
                               params.labelGap, params.centerX, params.centerY);
    } catch (const std::exception& ex) {
        // Keep current behavior: swallow and report; continue rendering the rest.
        std::cerr << "[Renderer] Ray draw failed: " << ex.what() << '\n';
    }
}

std::string Renderer::getConstellation(const std::string& baseLabel,
                                       const std::string& centerLabel,
                                       const std::vector<Ray>& upRays,
                                       const std::vector<Ray>& downRays,
                                       const Parameters& params) {
    Canvas canvas(params.canvasWidth, params.canvasHeight);

    // 1) Center star (fixed anchor).
    {
        Star center(centerLabel);
        center.draw(canvas, params.centerX, params.centerY);
    }

    // 2) Base star + horizontal connector to center.
    drawBaseAndConnector(canvas, baseLabel, params.centerX, params.centerY, kDesiredWire);

    // 3) Upward rays ('/') that start above-left and converge into center.
    for (const Ray& r : upRays) {
        drawRay(canvas, r, /*isUpward=*/true, params);
    }

    // 4) Downward rays ('\\') that start below-left and converge into center.
    for (const Ray& r : downRays) {
        drawRay(canvas, r, /*isUpward=*/false, params);
    }

    // 5) Return final ASCII image (trailing blanks trimmed per line).
    return canvas.getRender();
}
