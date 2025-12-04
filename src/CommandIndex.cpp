#include "CommandIndex.hpp"
#include "CommandTokenizer.hpp"
#include <iostream>

namespace stars {

void CommandIndex::addCommand(const std::string& line) {
  auto tokens = CommandTokenizer::getTokens(line);
  if (tokens.empty()) return;

  std::string base = tokens[0];

  // Check if variant already exists
  auto& variants = index_[base];
  for (auto& v : variants) {
    if (v.raw == line) {
      v.frequency++;
      return;
    }
  }

  // New variant
  CommandVariant variant;
  variant.raw = line;
  variant.tokens = tokens;

  // Count flags
  for (const auto& t : tokens) {
    if (!t.empty() && t[0] == '-') {
      variant.flagsCount++;
    }
  }

  variants.push_back(std::move(variant));
}

void CommandIndex::printSummary() const {
  for (const auto& [base, variants] : index_) {
    std::cout << base << ": " << variants.size() << " variants\n";
    for (const auto& v : variants) {
      std::cout << "  - " << v.raw << " (freq: " << v.frequency << ")\n";
    }
  }
}

}  // namespace stars
