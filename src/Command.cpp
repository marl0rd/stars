#include "Command.hpp"
#include <boost/tokenizer.hpp>
#include <algorithm>
#include <cctype>

using namespace stars;

Command Command::parseFromRaw(const std::string& rawLine) {
  Command cmd;
  cmd.raw_ = rawLine;

  auto rawTokens = tokenizeBoost(rawLine);
  cmd.tokens_ = normalize(rawTokens);

  return cmd;
}

std::string Command::getRaw() const { return raw_; }
const std::vector<std::string>& Command::getTokens() const { return tokens_; }
std::string Command::getBase() const { return tokens_.empty() ? std::string() : tokens_.front(); }

std::vector<std::string> Command::getFlagsSorted() const {
  std::vector<std::string> flags;
  if (tokens_.size() > 1) {
    for (std::size_t i = 1; i < tokens_.size(); ++i) {
      const auto& t = tokens_[i];
      if (!t.empty() && t[0] == '-') {
        flags.push_back(t);
      }
    }
  }
  std::sort(flags.begin(), flags.end());
  return flags;
}

std::vector<std::string> Command::getArgs() const {
  std::vector<std::string> args;
  if (tokens_.size() > 1) {
    for (std::size_t i = 1; i < tokens_.size(); ++i) {
      const auto& t = tokens_[i];
      if (t.empty()) continue;
      if (t[0] == '-') continue;  // skip flags
      args.push_back(t);
    }
  }
  return args;
}

int Command::getFlagsCount() const {
  int count = 0;
  if (tokens_.size() > 1) {
    for (std::size_t i = 1; i < tokens_.size(); ++i) {
      const auto& t = tokens_[i];
      if (!t.empty() && t[0] == '-') {
        ++count;
      }
    }
  }
  return count;
}

std::vector<std::string> Command::tokenizeBoost(const std::string& line) {
  boost::escaped_list_separator<char> sep('\\', ' ', '"');
  boost::tokenizer<boost::escaped_list_separator<char>> tok(line, sep);
  return std::vector<std::string>(tok.begin(), tok.end());
}

bool Command::isEnvAssignment(const std::string& token) {
  auto pos = token.find('=');
  if (pos == std::string::npos) return false;
  if (pos == 0) return false;  // "=VALUE" invalid name
  if (!token.empty() && token[0] == '-') return false;  // flags like -DFOO=BAR
  return true;
}

bool Command::isWrapper(const std::string& token) {
  // Common wrappers: sudo, env, command, time
  return (token == "sudo" || token == "env" || token == "command" || token == "time");
}

bool Command::isRedirection(const std::string& token) {
  if (token.empty()) return false;

  // Common operators (exact).
  if (token == ">" || token == ">>" || token == "<" || token == "<<" ||
      token == "&>" || token == "2>" || token == "1>" || token == "2>>" || token == "1>>") {
    return true;
  }
  // Glued redirections (e.g., ">>compose", ">file", "<input").
  if (token[0] == '>' || token[0] == '<') return true;

  // FD redirections like "2>file", "1>&2".
  if (std::isdigit(static_cast<unsigned char>(token[0])) &&
      token.size() >= 2 && token[1] == '>') {
    return true;
  }

  return false;
}

std::vector<std::string> Command::normalize(const std::vector<std::string>& tokens) {
  std::vector<std::string> out;
  out.reserve(tokens.size());

  std::size_t i = 0;

  // 1) Skip leading env assignments (NAME=VALUE).
  while (i < tokens.size() && isEnvAssignment(tokens[i])) {
    ++i;
  }

  // 2) Unwrap wrappers.
  if (i < tokens.size() && isWrapper(tokens[i])) {
    const std::string w = tokens[i];
    ++i;

    if (w == "env") {
      // Skip subsequent NAME=VALUE after 'env' until real command.
      while (i < tokens.size() && isEnvAssignment(tokens[i])) {
        ++i;
      }
    }
    // For sudo/command/time we just moved past the wrapper.
  }

  // 3) Copy remaining tokens, dropping redirections.
  for (; i < tokens.size(); ++i) {
    const std::string& t = tokens[i];
    if (isRedirection(t)) {
      continue;
    }
    out.push_back(t);
  }

  return out;
}
