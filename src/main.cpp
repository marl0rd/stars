#include <boost/program_options.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include "Configuration.hpp"
#include "Graph.hpp"
#include "History.hpp"
#include "Layout.hpp"
#include "Parser.hpp"
#include "Renderer.hpp"
#include "Terminal.hpp"

using namespace stars;
namespace po = boost::program_options;

int main(int argc, char** argv) {
    try {
        po::options_description desc("stars options");
        desc.add_options()("help,h", "Show help");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 0;
        }

        auto [termW, termH] = Terminal::getSize();
        auto historyPath = Terminal::getHistoryPath();

        std::size_t constellationLimit = 1;
        std::string constallationOrder = "frequency";

        Configuration config(historyPath, termW, termH, constellationLimit, constallationOrder);

        auto history = std::make_unique<History>();
        history->loadFromFile(config.getInputPath());

        auto parser = std::make_unique<Parser>();
        auto parsed = parser->parse(history->getRawLines());

        auto graph = std::make_unique<Graph>();
        graph->build(parsed);

        auto layout = std::make_unique<Layout>();
        layout->compute(*graph,
                        config.getWidth(),
                        config.getHeight(),
                        config.getMaxConstellations(),
                        config.getSortMode());

        auto renderer = std::make_unique<Renderer>();
        auto buffer = renderer->render(*graph, *layout);

        Terminal::write(buffer);
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "stars: " << ex.what() << "\n";
        return 1;
    }
}
