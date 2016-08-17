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
#include <boost/format.hpp>
#include "methods.hpp"
#include "printer.hpp"

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
 * JointPrinter.
 */

void JointPrinter::areChildPrinters(std::initializer_list<Printer *> childPrinters) {
  this->childPrinters = childPrinters;
}

JointPrinter::JointPrinter() { }

void JointPrinter::print(const std::string &str) {
  for (auto child : childPrinters) child->print(str);
}

void JointPrinter::setColor(const unsigned char r, const unsigned char g, const unsigned char b) {
  for (auto child : childPrinters) child->setColor(r, g, b);
}

void JointPrinter::breakLine() {
  for (auto child : childPrinters) child->breakLine();
}

/**
 * PrefixPrinter.
 */

void PrefixPrinter::verifyPrefix() {
  if (isNewLine) {
    prefix(base);
    isNewLine = false;
  }
}

PrefixPrinter::PrefixPrinter(Printer &base, PrintProcess prefix) :
    base(base), prefix(prefix), isNewLine(true) { }

void PrefixPrinter::print(const std::string &str) {
  verifyPrefix();
  base.print(str);
}

void PrefixPrinter::setColor(const unsigned char r,
                             const unsigned char g,
                             const unsigned char b) {
  verifyPrefix();
  base.setColor(r, b, g);
}

void PrefixPrinter::breakLine() {
  isNewLine = true;
  base.breakLine();
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

/**
 * LogPrinter.
 */

LogPrinter::LogPrinter() {
  std::stringstream filename;
  filename << "logs/log_" << std::time(NULL) << ".txt";
  fs::create_directories(fs::path("./logs/"));

  std::vector<spdlog::sink_ptr> sinks;
  sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
  sinks.push_back(std::make_shared<spdlog::sinks::simple_file_sink_mt>(filename.str().c_str(), true));
  mlogger = std::make_shared<spdlog::logger>("solemnsky", begin(sinks), end(sinks));

  // mlogger = std::make_shared<spdlog::logger>("solemnsky",
  // std::make_shared<spdlog::sinks::simple_file_sink_mt>(filename.str().c_str(),
  // true));
  mlogger->set_pattern("[%t | %C-%m-%d | %T] %v");
}

void LogPrinter::print(const std::string &str) {
  std::string mes = showOrigin(LogOrigin::None) + str;
  mlogger->info(mes);
}

void LogPrinter::print(const std::string &str, const LogOrigin &origin) {
  std::string mes = showOrigin(origin) + str;
  mlogger->info(mes);
}

namespace staticLogger {
LogPrinter lout;
}


/**
 * App logging functions
 */

void appLog(const std::string &contents, const LogOrigin origin) {
  //std::cout << showTime() << showOrigin(origin) << contents << "\n";
  staticLogger::lout.print(contents, origin);
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
    origin(origin),
    currentLine() { }

ConsolePrinter::~ConsolePrinter() {
  breakLine();
}

void ConsolePrinter::print(const std::string &str) {
  currentLine += str;
}

void ConsolePrinter::setColor(const unsigned char,
                              const unsigned char,
                              const unsigned char) {
  // TODO: console colors
}

void ConsolePrinter::breakLine() {
  if (currentLine.size() != 0) appLog(std::move(currentLine), origin);
  currentLine.clear();
}

/**
 * Printing.
 */

// TODO: is there a better way to implement this?

std::string printFloat(const float x) {
  std::stringstream str;
  str.precision(2);
  str << x;
  return str.str();
}

std::string printVector(const sf::Vector2f &vector) {
  return "(" + printFloat(vector.x) + ", " + printFloat(vector.y) + ")";
}

std::string printBool(const bool x) {
  return x ? "true" : "false";
}

std::string inQuotes(const std::string &str) {
  return "\"" + str + "\"";
}

std::string printTime(const Time delta) {
  std::stringstream str;
  str << boost::format("%.1f") % delta << "s";
  return str.str();
}

std::string printTimeDiff(const TimeDiff delta) {
  std::stringstream str;
  str << boost::format("%.2f") % (delta * 1000) << "ms";
  return str.str();
}

std::string printKbps(const Kbps rate) {
  std::stringstream str;
  str << boost::format("%.1f") % rate << "kBps";
  return str.str();
}
