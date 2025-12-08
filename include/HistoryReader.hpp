#pragma once

#include <optional>
#include <string>
#include <vector>

namespace stars {

/// @brief Reads bash history files and returns entries (bash raw lines).
class HistoryReader {
   public:
    struct Entry {
        std::string raw;
        std::optional<long long> timestamp;
    };

    const unsigned int LIMIT = 5000;

    HistoryReader() = default;

    // TODO: make configurable for other shells
    std::string getBashHistoryPath();
    std::vector<Entry> getEntries(const std::string& path);
};

}  // namespace stars
