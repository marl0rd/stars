#include <iostream>
#include <string>

/**
 * @brief CLI skeleton for stars (Milestone 1).
 */
namespace Stars {

static const char* kVersion = "0.1.0";

void printHelp() {
  std::cout << "stars - ASCII constellations from your shell history\n\n";
  std::cout << "Usage:\n";
  std::cout << "  stars [--histfile <path>] [--help] [--version]\n\n";
  std::cout << "Options:\n";
  std::cout << "  --help       Show this help and exit\n";
  std::cout << "  --version    Show version and exit\n";
  std::cout << "  --histfile   (placeholder) specify a history file path\n\n";
  std::cout << "Notes:\n";
  std::cout << "  Milestone 1 skeleton: CLI only. Implementation will follow in next milestones.\n";
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
      std::cout << "stars " << Stars::kVersion << "\n";
      return 0;
    } else if (arg == "--histfile" && i + 1 < argc) {
      histfile = argv[++i];
    } else {
      std::cerr << "Unknown argument: " << arg << "\n";
      std::cerr << "Use --help for usage.\n";
      return 1;
    }
  }

  // Default behavior for milestone 1: just print help if no args
  Stars::printHelp();
  return 0;
}
