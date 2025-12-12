#pragma once

#include <cstddef>
#include <string>

namespace stars {

class Configuration {
   public:
    Configuration(std::string inputPath,
                  std::size_t width,
                  std::size_t height,
                  std::size_t maxConstellations);

    std::string getInputPath() const;
    std::size_t getWidth() const;
    std::size_t getHeight() const;
    std::size_t getMaxConstellations() const;

   private:
    std::string inputPath_;
    std::size_t width_;
    std::size_t height_;
    std::size_t maxConstellations_;
};

}  // namespace stars
