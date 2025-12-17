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
/// - Base commands become central stars.
/// - Unique flag sets become variant stars attached to the base.
/// - Specialization chains connect earlier variants to later strict supersets.
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

    /// Build the constellation graph from normalized commands.
    void build(const std::vector<Command>& commands);

    const BoostGraph& getBoostGraph() const;
    std::vector<Vertex> getBaseVertices() const;
    std::vector<Vertex> getVariantsForBase(Vertex baseVertex) const;

   private:
    struct VariantUsage {
        std::string base;               ///< For each base
        std::set<std::string> flags;    ///< Collect a unique flag set
        std::size_t earliestIndex;      ///< Save first time seen
        std::size_t frequency = 0;      ///< And how often seen
    };

    struct VariantItem {
        Vertex vertex;
        std::size_t firstSeenIndex;
        std::size_t flagCount;
    };

    BoostGraph graph_;
    std::unordered_map<std::string, Vertex> baseVertices_;     // base -> vertex
    std::unordered_map<std::string, Vertex> variantVertices_;  // "base|f1,f2,..." -> vertex

    void clearState();

    static std::string makeKey(const std::string& base, const std::set<std::string>& flags);
    static std::string makeLabel(const std::string& base, const std::set<std::string>& flags);
    static bool isStrictSuperset(const std::set<std::string>& a, const std::set<std::string>& b);

    static std::vector<std::string> extractBases(const std::vector<Command>& commands);

    std::unordered_map<std::string, VariantUsage> collectVariantUsage(const std::vector<Command>& commands) const;

    void addBaseVertices(const std::vector<std::string>& bases);
    void addVariantVertices(const std::unordered_map<std::string, VariantUsage>& usageByKey);
    void addBaseToVariantEdges();

    std::vector<VariantItem> getVariantItemsForBase(const std::string& base) const;
    void linkSpecializationChainForBase(const std::string& base);

    static bool earlierByTime(const VariantItem& a, const VariantItem& b);
    static bool fewerFlagsThenEarlier(const VariantItem& a, const VariantItem& b);
};

}  // namespace stars