#pragma once
#include <string>
#include <vector>

#include "CommandIndex.hpp"
#include "graphics/Renderer.hpp"

namespace stars {

/// score = tokens + flags + 2×frequency + 0.25×char_length, clamped [2, 20]
int computeScore(int tokens, int flags, int frequency, int charLen);

/// Compact raw to "<...>" with ellipsis if needed.
std::string compact(const std::string& raw, int maxLen);

struct PlanParams {
    int maxLabelLen = 10;
};

struct ConstellationPlan {
    std::string baseLabel;      // Like: "<ls>"
    std::string centerLabel;    // chosen most frequent variant e.g., "<ls -a>"
    std::vector<Ray> upRays;    // Slash
    std::vector<Ray> downRays;  // Backslash
};

/// Build a plan for one base using exact variants (no normalization).
ConstellationPlan planOne(const std::string& base,
                          const std::vector<CommandVariant>& variants,
                          const PlanParams& p);

}  // namespace stars
