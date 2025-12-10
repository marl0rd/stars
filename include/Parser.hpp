#pragma once

#include <string>
#include <vector>

namespace stars {

/// A parsed command entry from history.
struct ParsedCommand {
    std::string original;            ///< Full command line.
    std::string base;                ///< Base command (first token).
    std::vector<std::string> flags;  ///< Tokens starting with '-'.
    std::vector<std::string> args;   ///< Other tokens (targets, etc.).
    std::size_t index;               ///< Chronological index.
};

/// Produces parsed commands from raw history.
class Parser {
   public:
    /// Parse raw lines into structured commands.
    std::vector<ParsedCommand> parse(const std::vector<std::string>& lines) const;

   private:
    static bool isSkippable(const std::string& line);
    static std::vector<std::string> tokenize(const std::string& line);
};

}  // namespace stars
