#include "History.hpp"

#include <fstream>
#include <stdexcept>

using namespace stars;

/// Load raw lines from a file. Throws on error.
///
/// We keep lines as-is; bash timestamps (lines starting with "# ") are
/// skipped later in Parser::isSkippable to preserve chronological index.
void History::loadFromFile(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error("Cannot open history file: " + path);
    }

    rawLines_.clear();
    std::string line;

    // Read all lines verbatim to retain ordering.
    while (std::getline(in, line)) {
        rawLines_.push_back(line);
    }
}

const std::vector<std::string>& History::getRawLines() const {
    return rawLines_;
}
