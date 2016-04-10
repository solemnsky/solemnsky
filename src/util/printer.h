/**
 * Abstraction of a thing that prints text, optionally with colors.
 */
#pragma once
#include <SFML/System.hpp>
#include <Box2D/Box2D.h>
#include <cereal/archives/json.hpp>
#include "types.h"

/**
 * Implemented by StringPrinter below and TextFrame in the Client UI.
 */
class Printer {
 public:
  virtual void print(const std::string &str) = 0;
  virtual void setColor(const unsigned char r,
                        const unsigned char g,
                        const unsigned char b) = 0;
  virtual void breakLine() = 0;
};

using PrintProcess = std::function<void(Printer &)>;

/**
 * Just prints to a string.
 */
class StringPrinter: public Printer {
 private:
  std::string value;
 public:
  StringPrinter() = default;

  void print(const std::string &str) override final;
  void setColor(const unsigned char,
                const unsigned char,
                const unsigned char) override final { }
  void breakLine() override final;

  std::string getString() const;
};

/**
 * Printer to the console, with neat formatting and origin information.
 */

enum class LogOrigin {
  None, // not specified
  Engine, // something deep in the engine
  Network, // network / enet stuff
  App, // multimedia management for clients
  Client, // misc. things for clients
  Server, // misc. things for servers
  Error // is a fatal error
};

class ConsolePrinter: public Printer {
 private:
  const static int timeLength = 10,
      originLength = 12;

  const LogOrigin origin;
  bool printedOrigin;

  std::string showTime() const;
  std::string showOrigin(const LogOrigin origin);

 public:
  ConsolePrinter(const LogOrigin origin);
  ~ConsolePrinter();

  void print(const std::string &str) override final;
  virtual void setColor(const unsigned char r,
                        const unsigned char g,
                        const unsigned char b) override final;
  virtual void breakLine() override final;
};

// note: ui::TextFrame implements Printer and
// ui::TextLog::print(PrintProcess) should be used for printing logs

/**
 * Logging / error throwing functions.
 */
void appLog(const std::string &contents, const LogOrigin = LogOrigin::None);
void appErrorLogic(const std::string &contents);
void appErrorRuntime(const std::string &contents);

/**
 * Log cereal-serializable types, hooray.
 */
template<typename T>
void appLogValue(const T &x) {
  std::stringstream stream;
  cereal::JSONOutputArchive archive(stream);
  archive(x);
  appLog(stream.str());
}