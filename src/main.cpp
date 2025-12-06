#include <boost/program_options.hpp>
#include <iostream>

#include "CommandIndex.hpp"
#include "HistoryReader.hpp"
#include "graphics/Renderer.hpp"
#include "ConstellationPlanner.hpp"

static constexpr const char* STARS_VERSION = "0.2.0";

int main(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description desc("stars Options:");
    desc.add_options()("help", "Show help message")("version", "Show version")("list", "Print the whole history")("constellation", po::value<std::string>(), "Render one constellation for the given base command");

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
            for (const auto& e : entries) std::cout << e.raw << '\n';
            return 0;
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
        for (const auto& e : entries) index.addCommand(e.raw);

        if (!vm.count("constellation")) {
            // Default: render the most active base command (by total frequency)
            const auto& cmap = index.map();
            if (cmap.empty()) {
                std::cerr << "No commands in history.\n";
                return 1;
            }
            // Pick base with highest cumulative variant frequency
            const auto* bestIt = &(*cmap.begin());
            int bestSum = 0;
            for (const auto& kv : cmap) {
                int sum = 0;
                for (const auto& v : kv.second) sum += v.frequency;
                if (sum > bestSum) {
                    bestSum = sum;
                    bestIt = &kv;
                }
            }
            const std::string base = bestIt->first;
            const auto& variants = bestIt->second;

            stars::PlanParams pp;  // label len default 10
            auto plan = stars::planOne(base, variants, pp);

            stars::RenderParams rp;  // uses default canvas and center
            std::cout << stars::Renderer::renderConstellation(
                plan.baseLabel, plan.centerLabel, plan.upRays, plan.downRays, rp);
            return 0;
        } else {
            const std::string base = vm["constellation"].as<std::string>();
            if (!index.hasBase(base)) {
                std::cerr << "Error: base command '" << base << "' not found in history.\n";
                return 1;
            }
            const auto& variants = index.variantsOf(base);

            stars::PlanParams pp;
            auto plan = stars::planOne(base, variants, pp);

            stars::RenderParams rp;  // default canvas & center
            std::cout << stars::Renderer::renderConstellation(
                plan.baseLabel, plan.centerLabel, plan.upRays, plan.downRays, rp);
            return 0;
        }
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
}
