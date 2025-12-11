#include "History.hpp"

#include <fstream>
#include <stdexcept>

using namespace stars;

/// Load raw lines from a file. Throws on error.
void History::loadFromFile(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error("Cannot open history file: " + path);
    }

    rawLines_.clear();
    std::string line;

    while (std::getline(in, line)) {
        rawLines_.push_back(line);
    }
}

const std::vector<std::string>& History::getLines() const {
    return rawLines_;
}
