#include "CommandParse.hpp"

#include <sstream>
#include <string>

namespace kitsune {

std::expected<CommandParse, std::string> CommandParse::parse(const std::string& in) {
    CommandParse out;

    bool hasCommandName = false;
    
    // TODO: This does not allow quotes to be escaped, but for now, I don't think it matters. The command system is
    // intentionally underspeced, so shouldn't be much of an issue
    for (size_t i = 0; i < in.size();) {
        if (hasCommandName && (in.at(i) == '"' || in.at(i) == '\'')) {
            auto nextQuote = in.find(in.at(i), i + 1);
            if (nextQuote == std::string::npos) {
                return std::unexpected("Command has unterminated quotes");
            }

            std::string word = in.substr(i + 1, nextQuote - i - 1);
            out.args.push_back(word);
            i = nextQuote + 1;
        } else if (in.at(i) == ' ') {
            ++i;
        } else {
            auto nextSpace = in.find(' ', i);
            auto nextIdx = nextSpace;
            if (nextSpace != std::string::npos) {

                nextSpace = nextSpace - i;
            }
            std::string word = in.substr(i, nextSpace);

            if (hasCommandName) {
                out.args.push_back(word);
            } else {
                out.command = word;
                hasCommandName = true;
            }
            
            i = nextIdx;
        }
    }
    return out;
}
}
