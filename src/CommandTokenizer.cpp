#include "CommandTokenizer.hpp"
#include <sstream>

namespace stars {

std::vector<std::string> CommandTokenizer::tokenize(const std::string& line) {
  std::vector<std::string> tokens;
  std::string current;
  bool inQuotes = false;
  char quoteChar = '\0';

  for (size_t i = 0; i < line.size(); ++i) {
    char c = line[i];

    if (inQuotes) {
      if (c == quoteChar) {
        inQuotes = false;
      } else {
        current.push_back(c);
      }
    } else {
      if (c == '"' || c == '\'') {
        inQuotes = true;
        quoteChar = c;
      } else if (std::isspace(static_cast<unsigned char>(c))) {
        if (!current.empty()) {
          tokens.push_back(current);
          current.clear();
        }
      } else {
        current.push_back(c);
      }
    }
  }

  if (!current.empty()) {
    tokens.push_back(current);
  }

  return tokens;
}

}  // namespace stars
