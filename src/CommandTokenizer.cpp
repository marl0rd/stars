#include "CommandTokenizer.hpp"

#include <boost/tokenizer.hpp>
#include <sstream>

namespace stars {

std::vector<std::string> CommandTokenizer::getTokens(const std::string& line) {
    boost::escaped_list_separator<char> sep('\\', ' ', '"');
    boost::tokenizer<boost::escaped_list_separator<char>> tok(line, sep);
    return std::vector<std::string>(tok.begin(), tok.end());
}

}  // namespace stars
