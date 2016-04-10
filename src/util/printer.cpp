#include "methods.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <sstream>
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
void appLog(const std::__cxx11::string &contents, const LogOrigin origin) {
  static std::vector<std::__cxx11::string> prefixes =
      {"]        : ",
       "engine]  : ",
       "network] : ",
       "app]     : ",
       "client]  : ",
       "server]  : ",
       "ERROR]   : ",
       "...      : "};

  std::stringstream stream(contents);
  std::__cxx11::string line;

  if (contents == "") {
    std::cout << showTime() + "[" + prefixes[(int) origin];
    endl(std::cout);
    return;
  }

  const std::__cxx11::string time = showTime();
  bool isFirstLine(true);
  while (getline(stream, line, '\n')) {
    if (isFirstLine) {
      std::cout << time + "[" + prefixes[(int) origin] + line;
      endl(std::cout);
      isFirstLine = false;
    } else {
      std::cout << time + prefixes[7] + line;
      endl(std::cout);
    }
  }
}
void appErrorLogic(const std::__cxx11::string &contents) {
  appLog(contents, LogOrigin::Error);
  throw logic_error(contents);
}
void appErrorRuntime(const std::__cxx11::string &contents) {
  appLog(contents, LogOrigin::Error);
  throw runtime_error(contents);
}