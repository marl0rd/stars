#pragma once

#include <string>
#include <vector>

#include "Graph.hpp"
#include "Layout.hpp"

namespace stars {

class Renderer {
   public:
    /// Render entire graph to a string buffer.
    std::string render(const Graph& graph, const Layout& layout) const;

   private:
    static void drawStar(std::vector<std::string>& canvas,
                         const Position& p,
                         const std::string& label);

    static void drawConnector(std::vector<std::string>& canvas,
                              const Position& a,
                              const Position& b);
};

}  // namespace stars
