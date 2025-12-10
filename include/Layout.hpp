#pragma once

#include <cstddef>
#include <unordered_map>
#include <utility>

#include "Graph.hpp"

namespace stars {

struct Position {
    std::size_t x;
    std::size_t y;
};

class Layout {
   public:
    /// Compute positions for all vertices, stacked vertically by base.
    void compute(const Graph& graph,
                 std::size_t width,
                 std::size_t height,
                 std::size_t maxConstellations,
                 const std::string& sortMode);

    /// Get position for a vertex (if present).
    Position getPosition(Vertex v) const;

    /// Return computed canvas size.
    std::pair<std::size_t, std::size_t> getCanvasSize() const;

    /// Return all assigned positions.
    const std::unordered_map<Vertex, Position>& getPositions() const;

   private:
    std::unordered_map<Vertex, Position> positions_;
    std::size_t canvasWidth_ = 0;
    std::size_t canvasHeight_ = 0;
};

}  // namespace stars
