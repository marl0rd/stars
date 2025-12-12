#include "Command.hpp"

#include <algorithm>
#include <boost/tokenizer.hpp>
#include <cctype>

using namespace stars;

Command::Command(): original(), base(), flags(), args(), index(0) {}

std::vector<Command> Command::parseLines(const std::vector<std::string>& lines) {
    std::vector<Command> out;
    out.reserve(lines.size());

    std::size_t idx = 0;
    for (const auto& line : lines) {
        if (isSkippableLine(line)) {
            ++idx;
            continue;
        }

        auto tokens = tokenizeBoost(line);

        Command cmd;
        cmd.original = line;
        cmd.base = extractBase(tokens);
        cmd.flags = extractFlagsSorted(tokens);
        cmd.args = extractArgs(tokens);
        cmd.index = idx;

        out.push_back(std::move(cmd));
        ++idx;
    }

    return out;
}

bool Command::isSkippableLine(const std::string& line) {
    if (line.empty()) return true;
    if (!line.empty() && line[0] == '#') return true;
    return false;
}

std::vector<std::string> Command::tokenizeBoost(const std::string& line) {
    boost::escaped_list_separator<char> sep('\\', ' ', '"');
    boost::tokenizer<boost::escaped_list_separator<char>> tok(line, sep);
    return std::vector<std::string>(tok.begin(), tok.end());
}

std::string Command::extractBase(const std::vector<std::string>& tokens) {
    if (tokens.empty()) return std::string();
    return tokens.front();
}

std::vector<std::string> Command::extractFlagsSorted(const std::vector<std::string>& tokens) {
    std::vector<std::string> flags;
    if (tokens.size() > 1) {
        for (std::size_t i = 1; i < tokens.size(); ++i) {
            const auto& t = tokens[i];
            if (!t.empty() && t[0] == '-') {
                flags.push_back(t);
            }
        }
    }
    std::sort(flags.begin(), flags.end());
    return flags;
}

std::vector<std::string> Command::extractArgs(const std::vector<std::string>& tokens) {
    std::vector<std::string> args;
    if (tokens.size() > 1) {
        for (std::size_t i = 1; i < tokens.size(); ++i) {
            const auto& t = tokens[i];
            if (t.empty()) continue;
            if (t[0] == '-') continue; 
            args.push_back(t);
        }
    }
    return args;
}
