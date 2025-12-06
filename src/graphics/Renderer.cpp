#include "graphics/Renderer.hpp"

namespace stars {

int Renderer::rightEdgeX(const Star& s, int sx) {
  return sx + s.drawWidth() - 1;
}

bool Renderer::placeLeftLabelThenWire(Canvas& canvas,
                                      const Star& s,
                                      int wireStartX, int wireStartY,
                                      DiagonalType type,
                                      int gap, int centerX, int centerY) {
  // Place star label so its right edge is 'gap' before wire start.
  int starX = wireStartX - gap - s.drawWidth();
  if (starX < 0) starX = 0; // keep inside canvas
  int starY = wireStartY;

  if (!s.draw(canvas, starX, starY)) return false;

  DiagonalWire wire(wireStartX, wireStartY, centerX, centerY, type);
  return wire.draw(canvas);
}

std::string Renderer::renderConstellation(const std::string& baseLabel,
                                          const std::string& centerLabel,
                                          const std::vector<Ray>& upRays,
                                          const std::vector<Ray>& downRays,
                                          const RenderParams& p) {
  Canvas canvas(p.canvasWidth, p.canvasHeight);

  // 1) Center star
  Star center(centerLabel);
  center.draw(canvas, p.centerX, p.centerY);

  // 2) Base on same row, connected horizontally to center
  Star base(baseLabel);
  int desiredWire = 20;
  int baseX = p.centerX - desiredWire - base.drawWidth();
  if (baseX < 0) baseX = 0;
  int baseY = p.centerY;
  base.draw(canvas, baseX, baseY);

  HorizontalWire hwire(rightEdgeX(base, baseX), p.centerX, baseY);
  hwire.draw(canvas);

  // 3) Up rays (Slash): start at (cx - len, cy - len)
  for (const auto& r : upRays) {
    int len = (r.len < 2) ? 2 : (r.len > 20 ? 20 : r.len);
    int sx = p.centerX - len;
    int sy = p.centerY - len;
    Star s(r.label);
    placeLeftLabelThenWire(canvas, s, sx, sy, DiagonalType::Slash, p.labelGap, p.centerX, p.centerY);
  }

  // 4) Down rays (Backslash): start at (cx - len, cy + len)
  for (const auto& r : downRays) {
    int len = (r.len < 2) ? 2 : (r.len > 20 ? 20 : r.len);
    int sx = p.centerX - len;
    int sy = p.centerY + len;
    Star s(r.label);
    placeLeftLabelThenWire(canvas, s, sx, sy, DiagonalType::Backslash, p.labelGap, p.centerX, p.centerY);
  }

  return canvas.render();
}

} // namespace stars
