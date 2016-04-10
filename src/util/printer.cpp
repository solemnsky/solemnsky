#include "methods.h"
#include "printer.h"

/**
 * StringPrinter.
 */

void StringPrinter::print(const std::string &str) {
  value += str;
}

void StringPrinter::breakLine() {
  value.push_back('\n');
}

std::string StringPrinter::getString() const {
  return value;
}

/**
 * ConsolePrinter.
 */

std::string ConsolePrinter::showTime() const {
  static sf::Clock clock;
  std::string clockTime =
      std::to_string(clock.getElapsedTime().asMilliseconds());
  return clockTime + std::string(timeLength - clockTime.size(), ' ');
}

std::string ConsolePrinter::showOrigin(const LogOrigin origin) {
  switch (origin) {
    case LogOrigin::None:
      return "[]        : ";
    case LogOrigin::Engine:
      return "[engine]  : ";
    case LogOrigin::Network:
      return "[network] : ";
    case LogOrigin::App:
      return "[app]     : ";
    case LogOrigin::Client:
      return "[client]  : ";
    case LogOrigin::Server:
      return "[server]  : ";
    case LogOrigin::Error:
      return "[error]   : ";
  }
}

ConsolePrinter::ConsolePrinter(const LogOrigin origin) :
    origin(origin), printedOrigin(false) {
  std::cout << showTime() + showOrigin(origin);
}

ConsolePrinter::~ConsolePrinter() {
  std::endl(std::cout);
}

void ConsolePrinter::print(const std::string &str) {
  std::cout << str;
}

void ConsolePrinter::setColor(const unsigned char r,
                              const unsigned char g,
                              const unsigned char b) {
  // TODO: console colors
}

void ConsolePrinter::breakLine() {
  endl(std::cout);
  std::cout << std::string(originLength + timeLength, ' ');
}

void appLog(const std::__cxx11::string &contents, const LogOrigin origin) {
  ConsolePrinter(origin).print(contents);
}

void appErrorLogic(const std::__cxx11::string &contents) {
  appLog(contents, LogOrigin::Error);
  throw std::logic_error(contents);
}
void appErrorRuntime(const std::__cxx11::string &contents) {
  appLog(contents, LogOrigin::Error);
  throw std::runtime_error(contents);
}