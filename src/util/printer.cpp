/**
 * solemnsky: the open-source multiplayer competitive 2D plane game
 * Copyright (C) 2016  Chris Gadzinski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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

void appErrorLogic(const std::string &contents) {
  appLog(contents, LogOrigin::Error);
  throw std::logic_error(contents);
}

void appErrorRuntime(const std::string &contents) {
  appLog(contents, LogOrigin::Error);
  throw std::runtime_error(contents);
}

/**
 * ConsolePrinter.
 */

ConsolePrinter::ConsolePrinter(const LogOrigin origin) :
    origin(origin) {}

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

/**
 * Printing.
 */

std::string printBool(const bool x) {
  return x ? "true" : "false";
}

