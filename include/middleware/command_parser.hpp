#ifndef COMMAND_PARSER_HPP
#define COMMAND_PARSER_HPP

#include <string_view>
// #include "optional"

enum class Command {
    STATUS,
    LAST_EVENT,
    CLEAR,
    HELP,
    UNKNOWN
};

class CommandParser {
public:
    static Command parseCommand(std::string_view line);
};

#endif // COMMAND_PARSER_HPP