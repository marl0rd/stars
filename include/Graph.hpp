#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "Parser.hpp"

namespace stars {

/// Vertex payload representing a star (base or variant).
struct StarVertexData {
    std::string label;               ///< Display label, e.g., "<ls -al>".
    std::string base;                ///< Base command (e.g., "ls").
    std::set<std::string> flags;     ///< Unique sorted flags for grouping.
    std::size_t frequency = 0;       ///< Occurrence count of identical lines.
    bool isBase = false;             ///< True if the central star.
    std::size_t firstSeenIndex = 0;  ///< Earliest history index for this node.
};

using BoostGraph = boost::adjacency_list<
    boost::vecS, boost::vecS, boost::directedS, StarVertexData>;

using Vertex = BoostGraph::vertex_descriptor;

class Graph {
   public:
    void build(const std::vector<ParsedCommand>& commands);

    const BoostGraph& getBoostGraph() const;

    std::vector<Vertex> getBaseVerticesOrdered(const std::string& sortMode) const;

    std::vector<Vertex> getVariantsForBase(Vertex baseVertex) const;

   private:
    // ---------- Data ----------
    BoostGraph graph_;
    std::unordered_map<std::string, Vertex> baseVertices_;
    std::unordered_map<std::string, Vertex> variantVertices_;

    // ---------- Helper types ----------
    struct VariantInfo {
        std::set<std::string> flags;
        std::size_t firstIndex;
    };

    void clearState();

    std::map<std::string, std::map<std::set<std::string>, std::size_t>>
    computeEarliestIndicesByBase(const std::vector<ParsedCommand>& commands) const;

    std::unordered_map<std::string, std::size_t>
    computeVariantFrequencies(const std::vector<ParsedCommand>& commands) const;

    Vertex createBaseVertex(const std::string& base);

    Vertex createVariantVertex(const std::string& base,
                               const std::set<std::string>& flags,
                               std::size_t firstIndex,
                               std::size_t frequency);

    void createBaseToVariantEdges();

    void createSpecializationEdges();

    void createSpecializationEdgesForBase(const std::string& base,
                                          const std::vector<Vertex>& variants);

    std::vector<Vertex> collectVariantVerticesForBase(const std::string& base) const;

    static bool isSuperset(const std::set<std::string>& smaller,
                           const std::set<std::string>& larger);

    static std::string makeKey(const std::string& base,
                               const std::set<std::string>& flags);

    static std::string makeLabel(const std::string& base,
                                 const std::set<std::string>& flags);

    std::size_t computeBaseFrequency(Vertex baseVertex) const;

    std::size_t computeBaseEarliestTime(Vertex baseVertex) const;
};

}  // namespace stars
