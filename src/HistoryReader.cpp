#include "HistoryReader.hpp"

#include <cstdlib>
#include <fstream>
#include <stdexcept>

namespace Stars {

std::string HistoryReader::autoDetectBashHistory() {
  const char* home = std::getenv("HOME");
  if (!home || std::string(home).empty()) {
    throw std::runtime_error("HOME is not set; cannot auto-detect ~/.bash_history.");
  }
  return std::string(home) + "/.bash_history";
}

std::vector<HistoryEntry> HistoryReader::readFile(const std::string& path) {
  std::ifstream in(path);
  if (!in) {
    throw std::runtime_error("Cannot open history file: " + path);
  }

  std::vector<HistoryEntry> entries;
  std::string line;
  entries.reserve(4096);  // small pre-reserve; grows as needed

  while (std::getline(in, line)) {
    if (line.empty()) continue;
    HistoryEntry e;
    e.raw = line;
    // Bash typically has no timestamps unless HISTTIMEFORMAT used (not in scope for v2).
    entries.emplace_back(std::move(e));
  }
  return entries;
}

}  // namespace Stars
