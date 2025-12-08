#include "HistoryReader.hpp"

#include <cstdlib>
#include <fstream>
#include <stdexcept>

using namespace stars;

std::string HistoryReader::getBashHistoryPath() {
    const char* home = std::getenv("HOME");
    if (!home || std::string(home).empty()) {
        throw std::runtime_error("HOME is not set; cannot auto-detect ~/.bash_history.");
    }
    return std::string(home) + "/.bash_history";
}

std::vector<HistoryReader::Entry> HistoryReader::getEntries(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("Cannot open history file: " + path);
    }

    std::vector<Entry> entries;
    std::string line;
    entries.reserve(LIMIT);

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        Entry e;
        e.raw = line;
        entries.emplace_back(std::move(e));
    }
    return entries;
}
