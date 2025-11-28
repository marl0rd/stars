#pragma once

#include <optional>
#include <string>
#include <vector>

namespace Stars {

/// @brief Represents a single bash history line.
class HistoryEntry {
   public:
    std::string raw;
    std::optional<long long> timestamp;  ///< Bash usually lacks timestamps; keep for future.
};

/// @brief Reads bash history files and returns entries.
class HistoryReader {
   public:
    HistoryReader() = default;

    /// @brief Auto-detects the default bash history path. @return Full path to ~/.bash_history
    std::string autoDetectBashHistory();

    /// @brief Reads a history file into entries. @return A list of HistoryEntry lines.
    std::vector<HistoryEntry> readFile(const std::string& path);
};

}  // namespace Stars
