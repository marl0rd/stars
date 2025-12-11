#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <limits>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "Command.hpp"

namespace stars {

/// Builds a constellation graph from normalized commands:
/// - Each base command becomes a central star.
/// - Variants (unique flag sets) attach to base.
/// - Specialization chains link earlier variants to later strict supersets.
class Graph {
   public:
    /// Payload for each vertex in the graph.
    struct StarVertexData {
        std::string label;               ///< Display label, e.g., "<ls -al>".
        std::string base;                ///< Base command (e.g., "ls").
        std::set<std::string> flags;     ///< Sorted unique flags defining a variant.
        std::size_t frequency = 0;       ///< Occurrence count (identical base+flags).
        bool isBase = false;             ///< True for central star.
        std::size_t firstSeenIndex = 0;  ///< Earliest history index for this node.
    };

    using BoostGraph =
        boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, StarVertexData>;
    using Vertex = BoostGraph::vertex_descriptor;

    /// Build the constellation graph from normalized commands.
    void build(const std::vector<Command>& commands);

    /// Access the underlying Boost graph.
    const BoostGraph& getBoostGraph() const;

    /// Order base vertices for display. Sort mode: "frequency" or "time".
    std::vector<Vertex> getBaseVerticesOrdered(const std::string& sortMode) const;

    /// Return variant vertices for a base, ordered by (fewer flags first, then earliest time).
    std::vector<Vertex> getVariantsForBase(Vertex baseVertex) const;

   private:
    BoostGraph graph_;
    std::unordered_map<std::string, Vertex> baseVertices_;     // base -> vertex
    std::unordered_map<std::string, Vertex> variantVertices_;  // key(base|flags) -> vertex

    // ---- Minimal helpers for cohesion ----
    static std::string makeKey(const std::string& base, const std::set<std::string>& flags);
    static std::string makeLabel(const std::string& base, const std::set<std::string>& flags);
    static bool isStrictSuperset(const std::set<std::string>& a, const std::set<std::string>& b);
    void clearState();
};

}  // namespace stars
