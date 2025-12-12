#pragma once

#include <string>
#include <vector>

namespace stars {

/// shell history lines
class History {
   public:

    History() = default;

    void loadFromFile(const std::string& path);
    const std::vector<std::string>& getLines() const;

   private:
    std::vector<std::string> rawLines_;
};

}  // namespace stars
