#pragma once

#include <string>
#include <vector>

#include "Graph.hpp"
#include "Layout.hpp"

namespace stars {

class Renderer {
   public:

   Renderer() = default;

    /// Render entire graph to a string buffer.
    std::string render(const Graph& graph, const Layout& layout);

   private:
   
   /*
                string(W)
          +-------------------+
      v   |   *               |
      e   |   | \             |
      c   |   |  *            |
      t   |   | / \           |
      o   |   *    *          |
      r   |                   |
     (H)  |                   |
          +-------------------+
   */
    std::vector<std::string> canvas;

    void drawStar(const Layout::Position& p, const std::string& label);

    void drawConnector(const Layout::Position& a, const Layout::Position& b);
};

}  // namespace stars
