#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <limits>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "Command.hpp"

namespace stars {

class Graph {
   public:

    struct StarVertex {
        std::string label;               ///< Display label, e.g., "<ls -al>".
        std::string base;                ///< Base command (e.g., "ls").
        std::set<std::string> flags;     ///< Sorted unique flags defining a variant.
        std::size_t frequency = 0;       ///< Occurrence count (identical base+flags).
        bool isBase = false;             ///< True for central star.
        std::size_t firstSeenIndex = 0;  ///< Earliest history index for this node.
    };

    using BoostGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, StarVertex>;
    using Vertex = BoostGraph::vertex_descriptor;

    Graph() = default;

    void build(const std::vector<Command>& commands);

    const BoostGraph& getBoostGraph() const;

    std::vector<Vertex> getBaseVertices() const;

    /// Return variant vertices for a base, ordered by (fewer flags first, then earliest time).
    std::vector<Vertex> getVariantsForBase(Vertex baseVertex) const;

   private:
    BoostGraph graph_;
    std::unordered_map<std::string, Vertex> baseVertices_;
    std::unordered_map<std::string, Vertex> variantVertices_;

    static std::string makeKey(const std::string& base, const std::set<std::string>& flags);
    static std::string makeLabel(const std::string& base, const std::set<std::string>& flags);
    static bool isStrictSuperset(const std::set<std::string>& a, const std::set<std::string>& b);
    void clearState();
};

}  // namespace stars
