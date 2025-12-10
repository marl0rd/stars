#pragma once

#include <string>
#include <vector>

namespace stars {

/// command parsed from bash history line.
class Command {
   public:
    static Command parseFromRaw(const std::string& rawLine);

    std::string getRaw() const;
    const std::vector<std::string>& getTokens() const;
    std::string getBase() const;
    std::vector<std::string> getFlagsSorted() const;
    std::vector<std::string> getArgs() const;
    int getFlagsCount() const;

   private:
    std::string raw_;
    std::vector<std::string> tokens_;

    static std::vector<std::string> tokenizeBoost(const std::string& line);
    static bool isEnvAssignment(const std::string& token);
    static bool isWrapper(const std::string& token);
    static bool isRedirection(const std::string& token);
    static std::vector<std::string> normalize(const std::vector<std::string>& tokens);
};

}  // namespace stars
