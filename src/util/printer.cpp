#include "printer.h"

void StringPrinter::print(const std::string &str) {
  value += str;
}

void StringPrinter::breakLine() {
  value.push_back('\n');
}

std::string StringPrinter::getString() const {
  return value;
}

std::string printToString(std::function<void(Printer &)> print) {
  StringPrinter p;
  print(p);
  return p.getString();
}

