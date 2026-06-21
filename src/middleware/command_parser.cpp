#include "middleware/command_parser.hpp"

Command CommandParser::parseCommand(std::string_view line) {
    if (line == "status") return Command::STATUS;
    if (line == "last_event") return Command::LAST_EVENT;
    if (line == "clear") return Command::CLEAR;
    if (line == "help") return Command::HELP;
    return Command::UNKNOWN;
}