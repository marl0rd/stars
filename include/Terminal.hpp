#pragma once

#include <cstddef>
#include <string>

namespace stars {

class Terminal {
   public:

    Terminal() = default;

    static std::pair<std::size_t, std::size_t> getSize();

    static void write(const std::string& buffer);

    static std::string getHistoryPath();
};

}  // namespace stars
