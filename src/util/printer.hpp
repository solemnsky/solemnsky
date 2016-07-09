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
/**
 * Printing text in ways appealing to humans.
 */
#pragma once
#include <ctime>
#include <SFML/System.hpp>
#include <Box2D/Box2D.h>
#include <cereal/archives/json.hpp>
#include <spdlog/spdlog.h>
#include "types.hpp"
#include <boost/filesystem.hpp>

// alias to boost filesystem library
namespace fs = boost::filesystem;

/**
 * Implemented by StringPrinter below and TextFrame in the Client UI.
 */
class Printer {
 public:
  virtual ~Printer() { }

  virtual void print(const std::string &str) = 0;
  virtual void setColor(const unsigned char r,
                        const unsigned char g,
                        const unsigned char b) = 0;
  virtual void breakLine() = 0;

  void printLn(const std::string &str) {
    print(str);
    breakLine();
  }

  void printTitle(const std::string &title) {
    setColor(0, 0, 255);
    printLn("** " + title + " **");
    setColor(255, 255, 255);
  }

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
 * Canonical console logging / error throwing.
 */

enum class LogOrigin {
  None, // not specified
  Engine, // something deep in the engine
  Network, // network / enet stuff
  App, // multimedia management for clients
  Client, // client multiplayer
  Server, // server multiplayer
  Error // is a fatal error
};

/**
 * A Printer subclass wrapping logging features
 */

class LogPrinter:public Printer {
public:
  LogPrinter();

  void print(const std::string &str) override final;
  void print(const std::string& str, const LogOrigin& origin);
  void setColor(const unsigned char r,
                        const unsigned char g,
                        const unsigned char b) override final{}
  void breakLine() override final{}
private:
  std::shared_ptr<spdlog::logger> mlogger;
};


/**
 * Logging functions
 */

void appLog(const std::string &contents, const LogOrigin = LogOrigin::None);
void appErrorRuntime(const std::string &contents);

/**
 * A printer wrapping appLog.
 */

class ConsolePrinter: public Printer {
private:
  const LogOrigin origin;
  std::string currentLine;

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
 * Log cereal-serializable types, hooray.
 */
template<typename T>
void appLogValue(const T &x) {
  std::stringstream stream;
  cereal::JSONOutputArchive archive(stream);
  archive(x);
  appLog(stream.str());
}

/**
 * Things helpful for printing / logging.
 */
std::string printFloat(const float x);
std::string printBool(const bool x);
std::string inQuotes(const std::string &str);
std::string printTime(const Time delta);
std::string printTimeDiff(const TimeDiff delta);
std::string printKbps(const Kbps rate);
