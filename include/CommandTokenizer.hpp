
#pragma once

#include <string>
#include <vector>

namespace stars {

/**
 * @brief Splits a command line into tokens, respecting quotes and escapes.
 */
class CommandTokenizer {
 public:
  /**
   * @brief Tokenizes a command line.
   * @param line Raw command line.
   * @return Vector of tokens.
   */
  static std::vector<std::string> tokenize(const std::string& line);
};

}  // namespace stars
