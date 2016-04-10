/**
 * Widget that displays a log created through the Printer interface.
 */
#pragma once
#include "ui/control.h"
#include "util/printer.h"

namespace ui {

namespace detail {

// plain data type, a vector of which can encode a PrintProcess
struct PrintAction {
 private:
  enum class Type {
    Color, Print, BreakLine
  } type;

  PrintAction(const Type type);
 public:
  optional<sf::Color> color;
  optional<std::string> string;

  void print(Printer &) const;

  static PrintAction Color(const sf::Color &color);
  static PrintAction Print(const std::string &string);
  static PrintAction BreakLine();
};

// a PrintProcess encoded into PrintActions
using PrintBlock = std::vector<PrintAction>;

class ActionPrinter: public Printer {
 private:
  std::vector<PrintAction> &printActions;

 public:
  ActionPrinter(std::vector<PrintAction> &printActions);

  void print(const std::string &str) override final;
  void setColor(const unsigned char r,
                const unsigned char g,
                const unsigned char b) override final;
  void breakLine() override final;

};

}

class TextLog: public Control {
 public:
  struct Style {
    float maxWidth, maxHeight, maxHeightCollapsed;
    float maxLifetime, maxLifetimeCollapsed, fadeStart;
    int fontSize;

    Style() = delete;
    Style(const float maxWidth,
          const float maxHeight,
          const float maxHeightCollapsed,
          const float maxLifetime,
          const float maxLifetimeCollapsed,
          const int fontSize);
  };

 private:
  Style style;
  sf::Vector2f pos;
  std::vector<std::pair<double, detail::PrintBlock>> printActions;

  TextFormat textFormat; // derived from style

 public:
  TextLog(AppState &appState, const Style &style, const sf::Vector2f &pos);

  // Control implementation
  void tick(float delta) override final;
  void render(Frame &f) override final;
  bool handle(const sf::Event &event) override final;

  // API
  void clear();
  void print(PrintProcess process);

  bool collapsed;
};

}

