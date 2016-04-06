#include "console.h"
#include <sstream>

ConsoleCommand::ConsoleCommand(const ConsoleCommand::Type type) :
    type(type) { }

optional<ConsoleCommand> ConsoleCommand::read(const std::string &str) {
  return {};
}
