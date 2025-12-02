#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "HistoryReader.hpp"
#unclude "CommandIndex.hpp"

static constexpr const char* STARS_VERSION = "0.2.0";

int main(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description desc("stars Options:");
    desc.add_options()("list", "Print the whole history")("help", "Show help message")("version", "Show version");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }
    if (vm.count("version")) {
        std::cout << "stars " << STARS_VERSION << "\n";
        return 0;
    }
    if (vm.count("list")) {
        try {
            stars::HistoryReader reader;
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

    try {
        stars::HistoryReader reader;
        std::string histfile = reader.getBashHistoryPath();
        auto entries = reader.readFile(histfile);

        stars::CommandIndex index;
        for (const auto& e : entries) {
            index.addCommand(e.raw);
        }
        index.printSummary();
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}
