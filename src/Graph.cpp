#include "Graph.hpp"

#include <algorithm>

using namespace stars;

void Graph::build(const std::vector<Command>& commands) {
    clearState();

    // For each command, collect unique bases
    auto bases = extractBases(commands);

    addBaseVertices(bases);

    // For each command, create a key and collect usage stats (usageByKey)
    auto usageByKey = collectVariantUsage(commands);

    // For each usageByKey, add variant vertex
    addVariantVertices(usageByKey);

    the problem i see is that all the keys are attached to the base
    also, the graph should filter the constelations, order the constellation and stuff... not the layout
    // Connect bases to variants
    addBaseToVariantEdges();

    // Specialization chains per base
    for (const auto& basePair : baseVertices_) {
        linkSpecializationChainForBase(basePair.first);
    }
}

void Graph::clearState() {
    graph_.clear();
    baseVertices_.clear();
    variantVertices_.clear();
}

void Graph::addBaseVertices(const std::vector<std::string>& bases) {
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
}

std::unordered_map<std::string, Graph::VariantUsage> Graph::collectVariantUsage(const std::vector<Command>& commands) const {
    std::unordered_map<std::string, VariantUsage> byKey;

    for (const auto& cmd : commands) {
        const std::string base = cmd.base;
        if (base.empty()) continue;

        // Build flag set (flags from Command are already sorted).
        const auto& flagsVec = cmd.flags;
        std::set<std::string> flagSet(flagsVec.begin(), flagsVec.end());

        const std::string key = makeKey(base, flagSet);
        const std::size_t idx = cmd.index;

        auto it = byKey.find(key);
        if (it == byKey.end()) {
            VariantUsage usage;
            usage.base = base;
            usage.flags = std::move(flagSet);
            usage.earliestIndex = idx;
            usage.frequency = 1;
            byKey.emplace(key, std::move(usage));
        } else {
            // Update earliest index and frequency.
            if (idx < it->second.earliestIndex) {
                it->second.earliestIndex = idx;
            }
            it->second.frequency += 1;
        }
    }

    return byKey;
}

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

bool Graph::isStrictSuperset(const std::set<std::string>& a, const std::set<std::string>& b) {
    if (a.size() >= b.size()) return false;
    return std::includes(b.begin(), b.end(), a.begin(), a.end());
}

std::vector<std::string> Graph::extractBases(const std::vector<Command>& commands) {
    std::set<std::string> uniques;
    for (const auto& cmd : commands) {
        const std::string base = cmd.base;
        if (!base.empty()) uniques.insert(base);
    }
    return std::vector<std::string>(uniques.begin(), uniques.end());
}

void Graph::addVariantVertices(const std::unordered_map<std::string, VariantUsage>& usageByKey) {
    for (const auto& kv : usageByKey) {
        const std::string& key = kv.first;
        const VariantUsage& usage = kv.second;

        Vertex v = boost::add_vertex(graph_);
        graph_[v].base = usage.base;
        graph_[v].flags = usage.flags;
        graph_[v].label = makeLabel(usage.base, usage.flags);
        graph_[v].isBase = false;
        graph_[v].firstSeenIndex = usage.earliestIndex;
        graph_[v].frequency = usage.frequency;

        variantVertices_[key] = v;
    }
}

void Graph::addBaseToVariantEdges() {
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
}

std::vector<Graph::VariantItem> Graph::getVariantItemsForBase(const std::string& base) const {
    std::vector<VariantItem> items;
    items.reserve(16);

    for (const auto& kv : variantVertices_) {
        const std::size_t pipePos = kv.first.find('|');
        const std::string keyBase =
            (pipePos == std::string::npos) ? kv.first : kv.first.substr(0, pipePos);

        if (keyBase == base) {
            Vertex v = kv.second;
            VariantItem item;
            item.vertex = v;
            item.firstSeenIndex = graph_[v].firstSeenIndex;
            item.flagCount = graph_[v].flags.size();
            items.push_back(item);
        }
    }

    return items;
}

bool Graph::earlierByTime(const VariantItem& a, const VariantItem& b) {
    return a.firstSeenIndex < b.firstSeenIndex;
}

bool Graph::fewerFlagsThenEarlier(const VariantItem& a, const VariantItem& b) {
    if (a.flagCount != b.flagCount) return a.flagCount < b.flagCount;
    return a.firstSeenIndex < b.firstSeenIndex;
}

void Graph::linkSpecializationChainForBase(const std::string& base) {
    // Gather and sort by time ascending.
    std::vector<VariantItem> items = getVariantItemsForBase(base);
    std::sort(items.begin(), items.end(), earlierByTime);

    // Link A -> B when B is the next later strict superset of A's flags.
    for (std::size_t i = 0; i < items.size(); ++i) {
        const auto& flagsA = graph_[items[i].vertex].flags;

        for (std::size_t j = i + 1; j < items.size(); ++j) {
            const auto& flagsB = graph_[items[j].vertex].flags;

            if (isStrictSuperset(flagsA, flagsB)) {
                boost::add_edge(items[i].vertex, items[j].vertex, graph_);
                break;  // Only chain to the next chronological superset.
            }
        }
    }
}

const Graph::BoostGraph& Graph::getBoostGraph() const {
    return graph_;
}

std::vector<Graph::Vertex> Graph::getBaseVertices() const {
    std::vector<Vertex> out;
    out.reserve(baseVertices_.size());
    for (const auto& kv : baseVertices_) {
        out.push_back(kv.second);
    }
    return out;
}

std::vector<Graph::Vertex> Graph::getVariantsForBase(Vertex baseVertex) const {
    const std::string baseName = graph_[baseVertex].base;

    std::vector<VariantItem> items = getVariantItemsForBase(baseName);
    std::sort(items.begin(), items.end(), fewerFlagsThenEarlier);

    std::vector<Vertex> out;
    out.reserve(items.size());
    for (const auto& it : items) out.push_back(it.vertex);
    return out;
}
