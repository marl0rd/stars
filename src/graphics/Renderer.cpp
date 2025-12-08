#include "graphics/Renderer.hpp"

#include <iostream>

using namespace stars;

int Renderer::rightEdgeX(const Star& s, int sx) {
    return sx + s.getDrawWidth() - 1;
}

void Renderer::placeLeftLabelThenWire(Canvas& canvas,
                                      const Star& s,
                                      int wireStartX, int wireStartY,
                                      int gap, int centerX, int centerY) {
    // Place star label so its right edge is 'gap' before wire start.
    int starX = wireStartX - gap - s.getDrawWidth();
    if (starX < 0) starX = 0;  // keep inside canvas
    int starY = wireStartY;

    s.draw(canvas, starX, starY);

    DiagonalWire wire(wireStartX, wireStartY, centerX, centerY);
    wire.draw(canvas);
}

std::string Renderer::getConstellation(const std::string& baseLabel,
                                       const std::string& centerLabel,
                                       const std::vector<Ray>& upRays,
                                       const std::vector<Ray>& downRays,
                                       const Parameters& params) {
    Canvas canvas(params.canvasWidth, params.canvasHeight);

    // 1) Center star
    Star center(centerLabel);
    center.draw(canvas, params.centerX, params.centerY);

    // 2) Base on same row, connected horizontally to center
    Star base(baseLabel);
    int desiredWire = 10;
    int baseX = params.centerX - desiredWire - base.getDrawWidth();
    if (baseX < 0) baseX = 0;
    int baseY = params.centerY;
    base.draw(canvas, baseX, baseY);

    HorizontalWire hwire(rightEdgeX(base, baseX), params.centerX, baseY);
    hwire.draw(canvas);

    // 3) Up rays (Slash): start at (cx - len, cy - len)
    for (const auto& r : upRays) {
        int len = (r.len < 2) ? 2 : (r.len > 10 ? 10 : r.len);
        int sx = params.centerX - len;
        int sy = params.centerY - len;
        Star s(r.label);
        try {
            placeLeftLabelThenWire(canvas, s, sx, sy, params.labelGap, params.centerX, params.centerY);
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }

    // 4) Down rays (Backslash): start at (cx - len, cy + len)
    for (const auto& r : downRays) {
        int len = (r.len < 2) ? 2 : (r.len > 10 ? 10 : r.len);
        int sx = params.centerX - len;
        int sy = params.centerY + len;
        Star s(r.label);
        try {
            placeLeftLabelThenWire(canvas, s, sx, sy, params.labelGap, params.centerX, params.centerY);
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }

    return canvas.getRender();
}
