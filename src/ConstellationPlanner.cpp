#include "ConstellationPlanner.hpp"

#include <algorithm>

using namespace stars;

int stars::computeScore(int tokens, int flags, int frequency, int charLen) {
    double s = tokens + flags + 2.0 * frequency + 0.25 * (double)charLen;
    if (s < 2.0) s = 2.0;
    if (s > 20.0) s = 20.0;
    return (int)s;
}

std::string stars::compact(const std::string& raw, int maxLen) {
    if ((int)raw.size() <= maxLen) return "<" + raw + ">";
    if (maxLen <= 3) return "<" + raw.substr(0, std::max(0, maxLen)) + ">";
    return "<" + raw.substr(0, maxLen - 3) + "...>";
}

ConstellationPlan stars::planOne(const std::string& base,
                          const std::vector<CommandVariant>& variants,
                          const PlanParams& p) {
    ConstellationPlan out;
    out.baseLabel = "<" + base + ">";
    if (variants.empty()) {
        out.centerLabel = out.baseLabel;
        return out;
    }

    // Center = most frequent; tie-break by (tokens+flags)
    auto best = std::max_element(variants.begin(), variants.end(),
                                 [](const CommandVariant& a, const CommandVariant& b) {
                                     if (a.frequency != b.frequency) return a.frequency < b.frequency;
                                     int sa = (int)a.tokens.size() + a.flagsCount;
                                     int sb = (int)b.tokens.size() + b.flagsCount;
                                     return sa < sb;
                                 });

    out.centerLabel = compact(best->raw, p.maxLabelLen);

    // Build rays from the rest; sort by descending score; then alternate up/down.
    struct Item {
        std::string label;
        int len;
    };
    std::vector<Item> items;
    items.reserve(variants.size() - 1);

    for (const auto& v : variants) {
        if (&v == &(*best)) continue;
        int len = computeScore((int)v.tokens.size(), v.flagsCount, v.frequency, (int)v.raw.size());
        items.push_back({compact(v.raw, p.maxLabelLen), len});
    }

    std::stable_sort(items.begin(), items.end(),
                     [](const Item& a, const Item& b) { return a.len > b.len; });

    bool toUp = true;
    for (const auto& it : items) {
        Ray r{it.label, it.len};
        if (toUp)
            out.upRays.push_back(r);
        else
            out.downRays.push_back(r);
        toUp = !toUp;
    }

    return out;
}
