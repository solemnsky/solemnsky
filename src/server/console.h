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

  static optional<ConsoleCommand> read(const std::string &str);

  std::vector<std::string> arguments;
};
