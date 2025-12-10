#include "Parser.hpp"

#include <algorithm>

#include "Command.hpp"

using namespace stars;

/// Returns true if the line should be skipped.
/// We ignore blank lines and bash timestamp lines ("# <epoch>") to keep order.
static bool isSkippableLocal(const std::string& line) {
    if (line.empty()) return true;
    if (!line.empty() && line[0] == '#') return true;  // HISTTIMEFORMAT timestamps
    return false;
}

std::vector<ParsedCommand> Parser::parse(const std::vector<std::string>& lines) const {
    std::vector<ParsedCommand> out;
    out.reserve(lines.size());

    std::size_t idx = 0;
    for (const auto& line : lines) {
        if (isSkippableLocal(line)) {
            ++idx;
            continue;
        }

        Command cmd = Command::parseFromRaw(line);
        const auto& tokens = cmd.getTokens();
        if (tokens.empty()) {
            ++idx;
            continue;
        }

        ParsedCommand pc;
        pc.original = cmd.getRaw();
        pc.base = cmd.getBase();
        pc.flags = cmd.getFlagsSorted();
        pc.args = cmd.getArgs();
        pc.index = idx;

        out.push_back(std::move(pc));
        ++idx;
    }

    return out;
}
