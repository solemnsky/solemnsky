/**
 * The server's remote console.
 */
#pragma once
#include "util/types.h"

struct ConsoleCommand {
 public:
  enum class Type {
    Echo
  } type;

 private:
  ConsoleCommand(const Type type);

 public:
  ConsoleCommand() = delete;
  ConsoleCommand(const std::string &clientCommand);

  std::vector<std::string> arguments;
};
