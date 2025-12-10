#include "Renderer.hpp"

#include <algorithm>

using namespace stars;

/// Draw a single star '*' and put its label to the right.
void Renderer::drawStar(std::vector<std::string>& canvas,
                        const Position& p,
                        const std::string& label) {
    if (p.y >= canvas.size()) return;
    auto& row = canvas[p.y];
    if (p.x >= row.size()) return;

    // Draw the star itself.
    row[p.x] = '*';

    // Draw label starting immediately after the star to avoid connector gaps.
    std::size_t lx = p.x + 1;
    for (char c : label) {
        if (lx >= row.size()) break;
        row[lx++] = c;
    }
}

/// Draw connector using '-', '/', '\' based on direction.
/// We step from a to b along a simple Bresenham-like line.
void Renderer::drawConnector(std::vector<std::string>& canvas,
                             const Position& a,
                             const Position& b) {
    int x0 = static_cast<int>(a.x);
    int y0 = static_cast<int>(a.y);
    int x1 = static_cast<int>(b.x);
    int y1 = static_cast<int>(b.y);

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        if (y0 >= 0 && y0 < static_cast<int>(canvas.size())) {
            auto& row = canvas[y0];
            if (x0 >= 0 && x0 < static_cast<int>(row.size())) {
                char ch = '-';
                if (dy != 0) {
                    ch = (sy > 0) ? '\\' : '/';
                }
                // Do not overwrite stars if already placed.
                if (row[x0] == ' ') row[x0] = ch;
            }
        }
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

/// Render graph to ASCII buffer following the layout.
/// We draw base->variant connectors and specialization chain edges.
std::string Renderer::render(const Graph& graph, const Layout& layout) const {
    auto [W, H] = layout.getCanvasSize();
    std::vector<std::string> canvas(H, std::string(W, ' '));

    const auto& g = graph.getBoostGraph();

    // Draw edges: first base->variant, then variant->variant (specialization).
    for (auto e : boost::make_iterator_range(boost::edges(g))) {
        Vertex src = boost::source(e, g);
        Vertex dst = boost::target(e, g);
        Position ps = layout.getPosition(src);
        Position pd = layout.getPosition(dst);
        drawConnector(canvas, ps, pd);
    }

    // Draw vertices last to avoid line overwrite.
    for (auto v : boost::make_iterator_range(boost::vertices(g))) {
        Position p = layout.getPosition(v);
        drawStar(canvas, p, g[v].label);
    }

    // Join lines.
    std::string out;
    out.reserve(H * (W + 1));
    for (const auto& row : canvas) {
        out.append(row);
        out.push_back('\n');
    }
    return out;
}
