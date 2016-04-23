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
 * AppLog.
 */

std::string showTime() {
  static sf::Clock clock;
  std::string clockTime =
      std::to_string(clock.getElapsedTime().asMilliseconds());
  return clockTime + std::string(10 - clockTime.size(), ' ');
}

std::string showOrigin(const LogOrigin origin) {
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
  throw enum_error();
}

void appLog(const std::string &contents, const LogOrigin origin) {
  std::cout << showTime() << showOrigin(origin) << contents << "\n";
}

void appErrorLogic(const std::__cxx11::string &contents) {
  appLog(contents, LogOrigin::Error);
  throw std::logic_error(contents);
}

void appErrorRuntime(const std::__cxx11::string &contents) {
  appLog(contents, LogOrigin::Error);
  throw std::runtime_error(contents);
}

/**
 * ConsolePrinter.
 */

ConsolePrinter::ConsolePrinter(const LogOrigin origin) :
    origin(origin) { }

ConsolePrinter::~ConsolePrinter() {
  breakLine();
}

void ConsolePrinter::print(const std::string &str) {
  currentLine += str;
}

void ConsolePrinter::setColor(const unsigned char r,
                              const unsigned char g,
                              const unsigned char b) {
  // TODO: console colors
}

void ConsolePrinter::breakLine() {
  if (currentLine.size() != 0) appLog(std::move(currentLine));
}

