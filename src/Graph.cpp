#include "Graph.hpp"

#include <algorithm>
#include <sstream>

using namespace stars;

std::string Graph::makeKey(const std::string& base,
                           const std::set<std::string>& flags) {
    std::string key = base;
    key.push_back('|');
    bool first = true;
    for (const auto& f : flags) {
        if (!first) key.push_back(',');
        key.append(f);
        first = false;
    }
    return key;
}

std::string Graph::makeLabel(const std::string& base, const std::set<std::string>& flags) {
    // Build label: "<base [-f1 -f2 ...]>"
    std::string label;
    label.reserve(base.size() + 4 + flags.size() * 4);  // pre-reserve a bit

    label.push_back('<');
    label.append(base);

    if (!flags.empty()) {
        label.push_back(' ');
        bool first = true;
        for (const auto& f : flags) {
            // Add space between flags to keep it readable.
            if (!first) {
                label.push_back(' ');
            }
            label.append(f);
            first = false;
        }
    }

    label.push_back('>');
    return label;
}

bool Graph::isSuperset(const std::set<std::string>& smaller,
                       const std::set<std::string>& larger) {
    if (smaller.size() > larger.size()) return false;
    return std::includes(larger.begin(), larger.end(), smaller.begin(), smaller.end());
}

// ---------- Build pipeline ----------

void Graph::clearState() {
    graph_.clear();
    baseVertices_.clear();
    variantVertices_.clear();
}

/// Compute earliest index for each (base, flags-set).
///
/// We use std::map for deterministic ordering, and std::set<std::string>
/// for the flags to ensure uniqueness and predictability.
std::map<std::string, std::map<std::set<std::string>, std::size_t>>
Graph::computeEarliestIndicesByBase(const std::vector<ParsedCommand>& commands) const {
    std::map<std::string, std::map<std::set<std::string>, std::size_t>> result;

    for (const auto& cmd : commands) {
        const std::string base = cmd.base;

        // Normalize flags to a set.
        std::set<std::string> flagSet(cmd.flags.begin(), cmd.flags.end());

        auto& byFlags = result[base];
        auto it = byFlags.find(flagSet);
        if (it == byFlags.end()) {
            byFlags.emplace(flagSet, cmd.index);
        } else {
            // Keep the earliest (smallest) index seen.
            it->second = std::min(it->second, cmd.index);
        }
    }

    return result;
}

/// Count occurrences for each (base, flags-set) across all commands.
/// This matches the requirement "Duplicates: skip identical commands; count frequency".
std::unordered_map<std::string, std::size_t>
Graph::computeVariantFrequencies(const std::vector<ParsedCommand>& commands) const {
    std::unordered_map<std::string, std::size_t> freq;

    for (const auto& cmd : commands) {
        const std::string base = cmd.base;

        // Normalize flags to a set to group variants.
        std::set<std::string> flagSet(cmd.flags.begin(), cmd.flags.end());

        const std::string key = makeKey(base, flagSet);
        auto it = freq.find(key);
        if (it == freq.end()) {
            freq.emplace(key, 1);
        } else {
            it->second += 1;
        }
    }

    return freq;
}

Vertex Graph::createBaseVertex(const std::string& base) {
    Vertex v = boost::add_vertex(graph_);
    graph_[v].label = "<" + base + ">";
    graph_[v].base = base;
    graph_[v].flags.clear();
    graph_[v].isBase = true;
    graph_[v].firstSeenIndex = std::numeric_limits<std::size_t>::max();
    graph_[v].frequency = 0;  // Not meaningful for base.
    baseVertices_[base] = v;
    return v;
}

Vertex Graph::createVariantVertex(const std::string& base,
                                  const std::set<std::string>& flags,
                                  std::size_t firstIndex,
                                  std::size_t frequency) {
    Vertex v = boost::add_vertex(graph_);
    auto& data = graph_[v];

    data.base = base;
    data.flags = flags;
    data.label = makeLabel(base, flags);
    data.isBase = false;
    data.firstSeenIndex = firstIndex;
    data.frequency = frequency;

    const std::string key = makeKey(base, flags);
    variantVertices_[key] = v;

    return v;
}

void Graph::createBaseToVariantEdges() {
    for (const auto& entry : variantVertices_) {
        const std::string& key = entry.first;
        Vertex variant = entry.second;

        // Extract base from "base|f1,f2,..."
        const std::size_t pipePos = key.find('|');
        std::string base = pipePos == std::string::npos ? key : key.substr(0, pipePos);

        auto baseIt = baseVertices_.find(base);
        if (baseIt == baseVertices_.end()) {
            // Defensive: should not happen if build() is consistent.
            continue;
        }
        Vertex baseV = baseIt->second;

        boost::add_edge(baseV, variant, graph_);
    }
}

void Graph::createSpecializationEdges() {
    // For each base, collect its variants and add A -> B where B is the next
    // chronological superset of A.
    for (const auto& baseEntry : baseVertices_) {
        const std::string& base = baseEntry.first;
        std::vector<Vertex> variants = collectVariantVerticesForBase(base);

        // Sort by first seen index for chronological chaining.
        std::sort(variants.begin(), variants.end(),
                  [this](Vertex a, Vertex b) {
                      return graph_[a].firstSeenIndex < graph_[b].firstSeenIndex;
                  });

        createSpecializationEdgesForBase(base, variants);
    }
}

void Graph::createSpecializationEdgesForBase(const std::string& base,
                                             const std::vector<Vertex>& variants) {
    (void)base;  // Base name is not needed for logic; kept for clarity.

    // For every variant A, find the next variant B that is a strict superset of A.
    for (std::size_t i = 0; i < variants.size(); ++i) {
        const auto& flagsA = graph_[variants[i]].flags;

        for (std::size_t j = i + 1; j < variants.size(); ++j) {
            const auto& flagsB = graph_[variants[j]].flags;

            if (flagsA == flagsB) {
                continue;  // Not a strict superset.
            }

            const bool superset = isSuperset(flagsA, flagsB);
            if (superset) {
                boost::add_edge(variants[i], variants[j], graph_);
                break;  // Chain to the next chronological superset only.
            }
        }
    }
}

// ---------- Public API ----------

/// Build the graph according to rules:
/// - Each base command becomes a central star.
/// - Variants (distinct flag sets) attach to base.
/// - Specialized variants (flag superset) chain chronologically.
/// - Duplicates: identical commands increment frequency; skip duplicate nodes.
void Graph::build(const std::vector<ParsedCommand>& commands) {
    clearState();

    // 1) Collect earliest indices per (base, flags) and frequencies per (base, flags).
    const auto earliestByBase = computeEarliestIndicesByBase(commands);
    const auto frequencies = computeVariantFrequencies(commands);

    // 2) Create base vertices.
    for (const auto& baseEntry : earliestByBase) {
        const std::string& base = baseEntry.first;
        createBaseVertex(base);
    }

    // 3) Create variant vertices for each base + flags-set with earliest index.
    for (const auto& baseEntry : earliestByBase) {
        const std::string& base = baseEntry.first;
        const auto& flagsMap = baseEntry.second;

        for (const auto& flagsEntry : flagsMap) {
            const std::set<std::string>& flags = flagsEntry.first;
            const std::size_t firstIdx = flagsEntry.second;

            const std::string key = makeKey(base, flags);

            // Frequency should exist; if not, default to 0 (defensive).
            std::size_t freq = 0;
            auto it = frequencies.find(key);
            if (it != frequencies.end()) {
                freq = it->second;
            }

            createVariantVertex(base, flags, firstIdx, freq);
        }
    }

    // 4) Connect base -> variant edges.
    createBaseToVariantEdges();

    // 5) Create specialization chains.
    createSpecializationEdges();
}

const BoostGraph& Graph::getBoostGraph() const {
    return graph_;
}

std::vector<Vertex> Graph::getBaseVerticesOrdered(const std::string& sortMode) const {
    std::vector<Vertex> bases;
    bases.reserve(baseVertices_.size());

    for (const auto& entry : baseVertices_) {
        bases.push_back(entry.second);
    }

    if (sortMode == "frequency") {
        // Sort by sum of variant frequencies, descending.
        std::sort(bases.begin(), bases.end(),
                  [this](Vertex a, Vertex b) {
                      const std::size_t fa = computeBaseFrequency(a);
                      const std::size_t fb = computeBaseFrequency(b);
                      return fa > fb;
                  });
    } else {
        // Sort by earliest variant time (ascending).
        std::sort(bases.begin(), bases.end(),
                  [this](Vertex a, Vertex b) {
                      const std::size_t ta = computeBaseEarliestTime(a);
                      const std::size_t tb = computeBaseEarliestTime(b);
                      return ta < tb;
                  });
    }

    return bases;
}

std::vector<Vertex> Graph::collectVariantVerticesForBase(const std::string& base) const {
    std::vector<Vertex> out;
    out.reserve(16);

    // Walk through variant key -> vertex map and keep those with the given base.
    for (const auto& entry : variantVertices_) {
        const std::string& key = entry.first;
        Vertex v = entry.second;

        const std::size_t pipePos = key.find('|');
        const std::string keyBase = pipePos == std::string::npos ? key : key.substr(0, pipePos);

        if (keyBase == base) {
            out.push_back(v);
        }
    }

    return out;
}

std::vector<Vertex> Graph::getVariantsForBase(Vertex baseVertex) const {
    std::vector<Vertex> out;

    const std::string baseName = graph_[baseVertex].base;
    out = collectVariantVerticesForBase(baseName);

    // Order variants: smaller flag count first, then earliest index.
    std::sort(out.begin(), out.end(),
              [this](Vertex a, Vertex b) {
                  const std::size_t sa = graph_[a].flags.size();
                  const std::size_t sb = graph_[b].flags.size();
                  if (sa != sb) return sa < sb;
                  return graph_[a].firstSeenIndex < graph_[b].firstSeenIndex;
              });

    return out;
}

// ---------- Aggregation helpers ----------

std::size_t Graph::computeBaseFrequency(Vertex baseVertex) const {
    const std::string baseName = graph_[baseVertex].base;
    std::size_t total = 0;

    for (auto v : boost::make_iterator_range(boost::vertices(graph_))) {
        const auto& data = graph_[v];
        if (!data.isBase && data.base == baseName) {
            total += data.frequency;
        }
    }

    return total;
}

std::size_t Graph::computeBaseEarliestTime(Vertex baseVertex) const {
    const std::string baseName = graph_[baseVertex].base;

    std::size_t earliest = std::numeric_limits<std::size_t>::max();

    for (auto v : boost::make_iterator_range(boost::vertices(graph_))) {
        const auto& data = graph_[v];
        if (!data.isBase && data.base == baseName) {
            if (data.firstSeenIndex < earliest) {
                earliest = data.firstSeenIndex;
            }
        }
    }

    // If no variants, keep max; callers should handle sorting with care.
    return earliest;
}
