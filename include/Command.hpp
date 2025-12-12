#pragma once

#include <string>
#include <vector>

namespace stars {

class Command {
   public:
    std::string original;
    std::string base;
    std::vector<std::string> flags;
    std::vector<std::string> args;
    std::size_t index;

    Command();

    /// Parse a list of raw history lines into normalized Command objects.
    static std::vector<Command> parseLines(const std::vector<std::string>& lines);

   private:
    static bool isSkippableLine(const std::string& line);
    static std::vector<std::string> tokenizeBoost(const std::string& line);

    static std::string extractBase(const std::vector<std::string>& normalized);
    static std::vector<std::string> extractFlagsSorted(const std::vector<std::string>& normalized);
    static std::vector<std::string> extractArgs(const std::vector<std::string>& normalized);
};

}  // namespace stars
