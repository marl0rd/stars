#pragma once

#include <optional>
#include <string>
#include <vector>

namespace Stars {

/// @brief Represents a single bash history line.
class HistoryEntry {
   public:
    std::string raw;
    std::optional<long long> timestamp;
};

/// @brief Reads bash history files and returns entries.
class HistoryReader {
   public:
    const unsigned int LIMIT = 5000;

    HistoryReader() = default;

    /// @brief Gets the default bash history file path (~/.bash_history).
    std::string getBashHistoryPath();

    /// @brief Reads a history file into entries. @return A list of HistoryEntry lines.
    std::vector<HistoryEntry> readFile(const std::string& path);
};

}  // namespace Stars
