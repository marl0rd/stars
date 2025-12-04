#pragma once

#include <string>
#include <vector>

namespace stars {

/// @brief Splits command into tokens, respecting quotes and escapes.
class CommandTokenizer {
   public:
    /// @brief Tokenizes a command line.
    static std::vector<std::string> getTokens(const std::string& line);
};

}  // namespace stars
