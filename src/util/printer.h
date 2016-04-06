/**
 * Abstraction of a thing that prints text, optionally with colors.
 */
#pragma once
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
