#include "Layout.hpp"

#include <algorithm>

using namespace stars;

/// Compute positions:
/// - Bases: stacked vertically, left margin.
/// - Variants: diagonals branching up/down alternating by order.
/// - Specialization chains: placed progressively farther right.
/// Keeps within canvas bounds.
void Layout::compute(const Graph& graph,
                     std::size_t width,
                     std::size_t height,
                     std::size_t maxConstellations) {
    positions_.clear();
    canvasWidth_ = width;
    canvasHeight_ = height;

    const std::size_t leftMargin = 1;
    const std::size_t horizStep = 8;         // columns between chain nodes
    const std::size_t vertStep = 1;          // rows between branches
    const std::size_t constellationGap = 6;  // rows between constellations
    std::size_t currentRow = 10;             // start row for first constellation

    auto bases = graph.getBaseVertices();
    if (bases.size() > maxConstellations) {
        bases.resize(maxConstellations);
    }

    for (std::size_t bi = 0; bi < bases.size(); ++bi) {
        auto base = bases[bi];

        // Place base.
        Position basePos{leftMargin, currentRow};
        positions_[base] = basePos;

        // Place variants alternating above/below diagonals.
        auto variants = graph.getVariantsForBase(base);

        // For stable diagonals, we track the chain levels by flag count.
        std::unordered_map<std::size_t, std::size_t> levelRightmostX;

        bool goUp = true;
        std::size_t branchIndex = 0;
        for (auto v : variants) {
            std::size_t level = graph.getBoostGraph()[v].flags.size();
            std::size_t baseX = basePos.x + (level * horizStep);
            std::size_t baseY = basePos.y;

            std::size_t dy = (branchIndex + 1) * vertStep;
            std::size_t y = goUp ? (baseY - dy) : (baseY + dy);
            goUp = !goUp;  // alternate

            // Clamp Y within canvas.
            if (y < 1) y = 1;
            if (y >= height - 2) y = height - 2;

            Position p{std::min(baseX, width - 4), y};
            positions_[v] = p;

            // Track rightmost x per level for specialization chaining.
            levelRightmostX[level] = std::max(levelRightmostX[level], p.x);

            ++branchIndex;
        }

        // Move row down for next constellation.
        currentRow += std::max<std::size_t>(constellationGap, (branchIndex + 1) * vertStep + 2);
        if (currentRow >= height - 1) {
            break;  // stop if out of canvas
        }
    }
}

Layout::Position Layout::getPosition(Graph::Vertex v) const {
    auto it = positions_.find(v);
    if (it != positions_.end()) return it->second;
    return Position{0, 0};
}

std::pair<std::size_t, std::size_t> Layout::getCanvasSize() const {
    return {canvasWidth_, canvasHeight_};
}

const std::unordered_map<Graph::Vertex, Layout::Position>& Layout::getPositions() const {
    return positions_;
}
