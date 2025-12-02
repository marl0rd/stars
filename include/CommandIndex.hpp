#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace stars {

/// @brief Represents a unique command variant.
struct CommandVariant {
  std::string raw;
  std::vector<std::string> tokens;
  size_t flagsCount = 0;
  size_t frequency = 1;
};

/// @brief Indexes commands by base command.
class CommandIndex {
 public:
  /// @brief Adds a command line to the index.
  void addCommand(const std::string& line);

  /// @brief TODO
  void printSummary() const;

 private:
  std::unordered_map<std::string, std::vector<CommandVariant>> index_;
};

}  // namespace stars
