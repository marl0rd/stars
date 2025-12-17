#include <gtest/gtest.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <iostream>

#include "Configuration.hpp"
#include "Graph.hpp"
#include "History.hpp"
#include "Layout.hpp"
#include "Renderer.hpp"
#include "Terminal.hpp"

using namespace stars;

TEST(MainTest, RunStars) {
    std::cout << "************ DEFAULT TEST RUNNING **********" << std::endl;
    auto history = std::make_unique<History>();
    auto graph = std::make_unique<Graph>();
    auto layout = std::make_unique<Layout>();
    auto renderer = std::make_unique<Renderer>();
    auto [termW, termH] = Terminal::getSize();
    auto historyPath = Terminal::getHistoryPath();
    auto testHistoryPath = "resources/.bash_history";
    auto constellationLimit = 1;

    Configuration config(testHistoryPath, termW, termH, constellationLimit);
    history->loadFromFile(config.getInputPath());
    graph->build(Command::parseLines(history->getLines()));
    layout->compute(*graph, config.getWidth(), config.getHeight(), config.getMaxConstellations());
    Terminal::write(renderer->render(*graph, *layout));
}
