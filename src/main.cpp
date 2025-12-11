#include <boost/program_options.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include "Configuration.hpp"
#include "Graph.hpp"
#include "History.hpp"
#include "Layout.hpp"
#include "Renderer.hpp"
#include "Terminal.hpp"

using namespace stars;
namespace po = boost::program_options;

int main(int argc, char** argv) {
    po::options_description desc("stars options");
    desc.add_options()("help,h", "Show help");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }

    auto history = std::make_unique<History>();
    auto graph = std::make_unique<Graph>();
    auto layout = std::make_unique<Layout>();
    auto renderer = std::make_unique<Renderer>();
    auto [termW, termH] = Terminal::getSize();
    auto historyPath = Terminal::getHistoryPath();
    auto constellationLimit = 1;
    auto constallationOrder = "frequency";

    Configuration config(historyPath, termW, termH, constellationLimit, constallationOrder);
    history->loadFromFile(config.getInputPath());
    graph->build(Command::parseLines(history->getLines()));
    layout->compute(*graph, config.getWidth(), config.getHeight(), config.getMaxConstellations(), config.getSortMode());
    Terminal::write(renderer->render(*graph, *layout));
    return 0;
}
