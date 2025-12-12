#include "Graph.hpp"

#include <algorithm>

/*
Definitions
  - Base: the command name (e.g., "ls", "git").
  - Flags: tokens starting with '-' (e.g., "-l", "-a"); sorted and unique.
  - Variant: a node representing (base + a distinct flag set).
  - Specialization: variant B strictly supersets variant A if flags(A) ⊂ flags(B),
    and the occurrence of B is later than A (chronology).
Rules
  1) Each base command becomes a central star: e.g., "<ls>", "<git>".
  2) Each unique flag set for a base becomes a variant star: e.g., "<ls -l>", "<ls -a>".
  3) If one variant's flags strictly specialize another (i.e., superset),
     * create a chain edge from the earlier variant to the later superset variant.
     * The chain respects chronology (firstSeenIndex).

Label convention:
  - Vertex labels are angle-bracketed, e.g. "<ls -al>".
  - Flags are space-separated (already sorted and unique).
  - Base-only stars have no flags: "<ls>".

Example
--------------
Given these history entries

  index  base  flags
  -----  ----  ---------------
   10    ls    [-t]
   15    ls    [-t, -a]
   20    ls    [-t, -a, -l]

Aggregation yields three variant nodes:
  <ls -t>          (earliest 10, frequency 1)
  <ls -t -a>       (earliest 15, frequency 1)
  <ls -t -a -l>    (earliest 20, frequency 1)

Edges:
  Base "<ls>" → each variant (branches)
  Chronological specialization chain:
    <ls -t>    → <ls -t -a>       (since {-t} ⊂ {-t, -a})
    <ls -t -a> → <ls -t -a -l>    (since {-t, -a} ⊂ {-t, -a, -l})
*/

using namespace stars;

/// Sort pair<VariantVertex, metric> by metric ascending.
static bool compareBySecondAsc(
    const std::pair<Graph::Vertex, std::size_t>& a,
    const std::pair<Graph::Vertex, std::size_t>& b) {
    return a.second < b.second;
}

/// Create a unique key for (base, flags-set).
std::string Graph::makeKey(const std::string& base, const std::set<std::string>& flags) {
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

/// Create display label for (base, flags-set).
std::string Graph::makeLabel(const std::string& base, const std::set<std::string>& flags) {
    std::string label;
    label.push_back('<');
    label.append(base);
    if (!flags.empty()) {
        label.push_back(' ');
        bool first = true;
        for (const auto& f : flags) {
            if (!first) label.push_back(' ');
            label.append(f);
            first = false;
        }
    }
    label.push_back('>');
    return label;
}

/// Check if set a is a strict superset of set b.
bool Graph::isStrictSuperset(const std::set<std::string>& a, const std::set<std::string>& b) {
    if (a.size() >= b.size()) return false;
    return std::includes(b.begin(), b.end(), a.begin(), a.end());
}

/// Clear all graph state.
void Graph::clearState() {
    graph_.clear();
    baseVertices_.clear();
    variantVertices_.clear();
}

void Graph::build(const std::vector<Command>& commands) {
    /// 1) Create base vertices.
    /// 2) Aggregate variants (base+flags) -> earliest index + frequency.
    /// 3) Create variant vertices with labels.
    /// 4) Add base->variant edges.
    /// 5) For each base, order variants chronologically and chain to next strict superset.

    clearState();

    // 1) Collect all bases and create base vertices.
    std::set<std::string> bases;
    for (const auto& cmd : commands) {
        const std::string b = cmd.base;
        if (!b.empty()) bases.insert(b);
    }
    for (const auto& base : bases) {
        Vertex v = boost::add_vertex(graph_);
        graph_[v].label = "<" + base + ">";
        graph_[v].base = base;
        graph_[v].flags.clear();
        graph_[v].frequency = 0;
        graph_[v].isBase = true;
        graph_[v].firstSeenIndex = std::numeric_limits<std::size_t>::max();
        baseVertices_[base] = v;
    }

    // 2) Aggregate variants for (base, flags-set).
    struct Agg {
        std::string base;
        std::set<std::string> flags;
        std::size_t freq = 0;
        std::size_t earliest = std::numeric_limits<std::size_t>::max();
    };
    std::unordered_map<std::string, Agg> aggByKey;

    // Checks if variant key exists, else creates new aggregation entry.
    for (const auto& cmd : commands) {
        const std::string base = cmd.base;
        if (base.empty()) continue;

        std::set<std::string> flagSet(cmd.flags.begin(), cmd.flags.end());
        const std::string key = makeKey(base, flagSet);

        auto it = aggByKey.find(key);
        if (it == aggByKey.end()) {
            Agg a;
            a.earliest = cmd.index;
            a.freq = 1;
            a.flags = flagSet;
            a.base = base;
            aggByKey.emplace(key, std::move(a));
        } else {
            if (cmd.index < it->second.earliest) {
                it->second.earliest = cmd.index;
            }
            it->second.freq += 1;
        }
    }

    // 3) Create variant vertices with labels and store in map.
    for (const auto& kv : aggByKey) {
        const Agg& a = kv.second;

        Vertex v = boost::add_vertex(graph_);
        graph_[v].base = a.base;
        graph_[v].flags = a.flags;
        graph_[v].label = makeLabel(a.base, a.flags);
        graph_[v].isBase = false;
        graph_[v].firstSeenIndex = a.earliest;
        graph_[v].frequency = a.freq;

        variantVertices_[kv.first] = v;
    }

    // 4) Connect base -> variant edges.
    for (const auto& kv : variantVertices_) {
        const std::string& key = kv.first;
        Vertex variant = kv.second;

        const std::size_t pipePos = key.find('|');
        const std::string base = (pipePos == std::string::npos) ? key : key.substr(0, pipePos);

        auto itBase = baseVertices_.find(base);
        if (itBase != baseVertices_.end()) {
            boost::add_edge(itBase->second, variant, graph_);
        }
    }

    // 5) Specialization chains: chronological order + strict superset of flags.
    for (const auto& basePair : baseVertices_) {
        const std::string& base = basePair.first;

        // Gather variants for this base with their earliest index.
        std::vector<std::pair<Vertex, std::size_t>> variantsWithTime;
        variantsWithTime.reserve(16);

        for (const auto& kv : variantVertices_) {
            const std::size_t pipePos = kv.first.find('|');
            const std::string keyBase =
                (pipePos == std::string::npos) ? kv.first : kv.first.substr(0, pipePos);
            if (keyBase == base) {
                Vertex v = kv.second;
                variantsWithTime.emplace_back(v, graph_[v].firstSeenIndex);
            }
        }

        // Order by time ascending.
        std::sort(variantsWithTime.begin(), variantsWithTime.end(), compareBySecondAsc);

        // For each variant A, link to next later variant B that strictly supersets A's flags.
        for (std::size_t i = 0; i < variantsWithTime.size(); ++i) {
            Vertex va = variantsWithTime[i].first;
            const auto& flagsA = graph_[va].flags;

            for (std::size_t j = i + 1; j < variantsWithTime.size(); ++j) {
                Vertex vb = variantsWithTime[j].first;
                const auto& flagsB = graph_[vb].flags;

                if (isStrictSuperset(flagsA, flagsB)) {
                    boost::add_edge(va, vb, graph_);
                    break;  // chain only to the next chronological superset
                }
            }
        }
    }
}

const Graph::BoostGraph& Graph::getBoostGraph() const { return graph_; }

std::vector<Graph::Vertex> Graph::getBaseVertices() const {
    std::vector<std::pair<Vertex, std::size_t>> basesWithMetric;
    basesWithMetric.reserve(baseVertices_.size());

    // Precompute metrics: frequency sum or earliest time.
    for (const auto& kv : baseVertices_) {
        Vertex bv = kv.second;
        const std::string& baseName = graph_[bv].base;

        std::size_t metric = 0;
        // Earliest variant time for this base.
        metric = std::numeric_limits<std::size_t>::max();
        for (auto v : boost::make_iterator_range(boost::vertices(graph_))) {
            const auto& d = graph_[v];
            if (!d.isBase && d.base == baseName) {
                if (d.firstSeenIndex < metric) metric = d.firstSeenIndex;
            }
        }

        basesWithMetric.emplace_back(bv, metric);
    }
    // Sort by metric ascending (earliest time first).
    std::sort(basesWithMetric.begin(), basesWithMetric.end(), compareBySecondAsc);

    // Strip metrics, return vertex order.
    std::vector<Vertex> out;
    out.reserve(basesWithMetric.size());
    for (const auto& p : basesWithMetric) out.push_back(p.first);
    return out;
}

std::vector<Graph::Vertex> Graph::getVariantsForBase(Vertex baseVertex) const {
    std::vector<std::pair<Vertex, std::pair<std::size_t, std::size_t>>> items;  // v, (flagCount, time)
    const std::string baseName = graph_[baseVertex].base;

    for (auto v : boost::make_iterator_range(boost::vertices(graph_))) {
        const auto& d = graph_[v];
        if (!d.isBase && d.base == baseName) {
            std::size_t fc = d.flags.size();
            std::size_t t = d.firstSeenIndex;
            items.emplace_back(v, std::make_pair(fc, t));
        }
    }

    // Sort by fewer flags first, then earliest time.
    for (std::size_t i = 0; i + 1 < items.size(); ++i) {
        for (std::size_t j = i + 1; j < items.size(); ++j) {
            const auto& ai = items[i].second;
            const auto& aj = items[j].second;
            bool swapNeeded = false;

            if (ai.first > aj.first) {
                swapNeeded = true;  // more flags should go later
            } else if (ai.first == aj.first && ai.second > aj.second) {
                swapNeeded = true;  // same flags count: later time should go later
            }

            if (swapNeeded) {
                std::swap(items[i], items[j]);
            }
        }
    }

    std::vector<Vertex> out;
    out.reserve(items.size());
    for (const auto& it : items) out.push_back(it.first);
    return out;
}
