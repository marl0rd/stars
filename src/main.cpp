
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "HistoryReader.hpp"

namespace Stars {
static constexpr const char* VERSION = "0.2.0";
void printHelp() {
    std::cout << "stars - ASCII constellations from your bash history\n\n";
    std::cout << "Usage:\n";
    std::cout << "  stars [--histfile <path>] [--help] [--version]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --list       Print the whole history\n";
    std::cout << "  --help       Show this help and exit\n";
    std::cout << "  --version    Show version and exit\n";
    std::cout << "  --histfile   Specify a bash history file path (default: ~/.bash_history)\n\n";
}
}  // namespace Stars

int main(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");
    desc.add_options()("list", "Print the whole history")("help", "Show help message")("version", "Show version");

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (const std::exception& ex) {
        std::cerr << "Error parsing options: " << ex.what() << '\n';
        std::cerr << "Use --help for usage." << '\n';
        return 1;
    }

    if (vm.count("help")) {
        Stars::printHelp();
        return 0;
    }

    if (vm.count("version")) {
        std::cout << "stars " << Stars::VERSION << "\n";
        return 0;
    }

    if (vm.count("list")) {
        try {
            Stars::HistoryReader reader;
            std::string histfile = reader.getBashHistoryPath();

            auto entries = reader.readFile(histfile);

            for (const auto& e : entries) {
                std::cout << e.raw << '\n';
            }
        } catch (const std::exception& ex) {
            std::cerr << "Error: " << ex.what() << '\n';
            return 1;
        }
    }

    return 0;
}
