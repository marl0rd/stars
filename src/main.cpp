
#include <iostream>
#include <string>
#include <vector>

#include "HistoryReader.hpp"

/**
 * @brief CLI for stars Milestone 2: read bash history and print lines.
 */
namespace Stars {
static constexpr const char* VERSION = "0.2.0";
void printHelp() {
  std::cout << "stars - ASCII constellations from your bash history\n\n";
  std::cout << "Usage:\n";
  std::cout << "  stars [--histfile <path>] [--help] [--version]\n\n";
  std::cout << "Options:\n";
  std::cout << "  --help       Show this help and exit\n";
  std::cout << "  --version    Show version and exit\n";
  std::cout << "  --histfile   Specify a bash history file path (default: ~/.bash_history)\n\n";
}
}  // namespace Stars

int main(int argc, char** argv) {
  std::string histfile;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--help") {
      Stars::printHelp();
      return 0;
    } else if (arg == "--version") {
      std::cout << "stars " << Stars::VERSION << "\n";
      return 0;
    } else if (arg == "--histfile" && i + 1 < argc) {
      histfile = argv[++i];
    } else {
      std::cerr << "Unknown argument: " << arg << "\n";
      std::cerr << "Use --help for usage.\n";
      return 1;
    }
  }

  try {
    Stars::HistoryReader reader;
    if (histfile.empty()) {
      histfile = reader.autoDetectBashHistory();
    }

    auto entries = reader.readFile(histfile);

    // Milestone 2 behavior: print raw lines. (We’ll add parsing in Milestone 3.)
    for (const auto& e : entries) {
      std::cout << e.raw << '\n';
    }
  } catch (const std::exception& ex) {
    std::cerr << "Error: " << ex.what() << '\n';
    return 1;
  }

  return 0;
}
