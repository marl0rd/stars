#include "Renderer.hpp"

#include <algorithm>

using namespace stars;

/// Draw a single star '*' and put its label to the right.
void Renderer::drawStar(const Layout::Position& p, const std::string& label) {
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

void Renderer::drawConnector(const Layout::Position& a, const Layout::Position& b) {
    // --- Convert positions to integer grid coordinates ---
    const int x0 = static_cast<int>(a.x);
    const int y0 = static_cast<int>(a.y);
    const int x1 = static_cast<int>(b.x);
    const int y1 = static_cast<int>(b.y);

    // --- Compute deltas and the number of steps for DDA ---
    const int dx = x1 - x0;
    const int dy = y1 - y0;
    const int adx = (dx >= 0) ? dx : -dx;
    const int ady = (dy >= 0) ? dy : -dy;
    const int steps = (adx > ady) ? adx : ady;

    // If both points are the same, nothing to draw (or a single tick if desired).
    if (steps == 0) {
        if (y0 >= 0 && y0 < static_cast<int>(canvas.size())) {
            auto& row = canvas[y0];
            if (x0 >= 0 && x0 < static_cast<int>(row.size())) {
                if (row[x0] == ' ') {
                    row[x0] = '-';  // minimal mark for a degenerate connector
                }
            }
        }
        return;
    }

    // --- Choose one glyph for the whole connector based on overall direction ---
    // Horizontal: '-', Vertical: '|', Diagonal down: '\', Diagonal up: '/'
    char glyph;
    if (dy == 0) {
        glyph = '-';
    } else if (dx == 0) {
        glyph = '|';
    } else {
        glyph = (dy > 0) ? '\\' : '/';
    }

    // --- DDA increments per step (floating point for clarity) ---
    const double incX = static_cast<double>(dx) / static_cast<double>(steps);
    const double incY = static_cast<double>(dy) / static_cast<double>(steps);

    // --- Iterate along the segment and plot only into empty cells ---
    double xf = static_cast<double>(x0);
    double yf = static_cast<double>(y0);

    for (int i = 0; i <= steps; ++i) {
        // Round to nearest integer grid cell.
        const int xi = static_cast<int>(std::round(xf));
        const int yi = static_cast<int>(std::round(yf));

        // Bounds check per point.
        if (yi >= 0 && yi < static_cast<int>(canvas.size())) {
            auto& row = canvas[yi];
            if (xi >= 0 && xi < static_cast<int>(row.size())) {
                // Do not overwrite stars or labels: only draw into empty space.
                if (row[xi] == ' ') {
                    row[xi] = glyph;
                }
            }
        }

        xf += incX;
        yf += incY;
    }
}

/// Render graph to ASCII buffer following the layout.
/// We draw base->variant connectors and specialization chain edges.
std::string Renderer::render(const Graph& graph, const Layout& layout) {
    // Prepare blank canvas.
    auto [W, H] = layout.getCanvasSize();
    for(std::size_t i = 0; i < H; ++i) {
        canvas.emplace_back(std::string(W, ' '));
    }

    const auto& g = graph.getBoostGraph();

    // Draw edges: first base->variant, then variant->variant (specialization).
    for (auto e : boost::make_iterator_range(boost::edges(g))) {
        Graph::Vertex src = boost::source(e, g);
        Graph::Vertex dst = boost::target(e, g);
        Layout::Position ps = layout.getPosition(src);
        Layout::Position pd = layout.getPosition(dst);
        drawConnector(ps, pd);
    }

    // Draw vertices last to avoid line overwrite.
    for (auto v : boost::make_iterator_range(boost::vertices(g))) {
        Layout::Position p = layout.getPosition(v);
        drawStar(p, g[v].label);
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
