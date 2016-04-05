#include "console.h"
#include <sstream>

ConsoleCommand::ConsoleCommand(const ConsoleCommand::Type type) :
    type(type) { }

ConsoleCommand::ConsoleCommand(const std::string &clientCommand) {

}


