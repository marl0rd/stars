#include "Configuration.hpp"

using namespace stars;

Configuration::Configuration(std::string inputPath,
                             std::size_t width,
                             std::size_t height,
                             std::size_t maxConstellations,
                             std::string sortMode)
    : inputPath_(std::move(inputPath)),
      width_(width),
      height_(height),
      maxConstellations_(maxConstellations),
      sortMode_(std::move(sortMode)) {}

std::string Configuration::getInputPath() const { return inputPath_; }
std::size_t Configuration::getWidth() const { return width_; }
std::size_t Configuration::getHeight() const { return height_; }
std::size_t Configuration::getMaxConstellations() const { return maxConstellations_; }
std::string Configuration::getSortMode() const { return sortMode_; }
