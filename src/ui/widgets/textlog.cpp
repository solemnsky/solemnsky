#include "textlog.h"

namespace ui {

namespace detail {

/**
 * PrintAction.
 */

PrintAction::PrintAction(const PrintAction::Type type) :
    type(type) { }

void PrintAction::print(Printer &p) {
  switch (type) {
    case Type::Color: {
      p.setColor(color->r, color->g, color->b);
      break;
    }
    case Type::Print: {
      p.print(string.get());
      break;
    }
    case Type::BreakLine: {
      p.breakLine();
      break;
    }
  }
}

PrintAction PrintAction::Color(const sf::Color &color) {
  PrintAction action(Type::Color);
  action.color = color;
  return action;
}

PrintAction PrintAction::Print(const std::string &string) {
  PrintAction action(Type::Print);
  action.string = string;
  return action;
}

PrintAction PrintAction::BreakLine() {
  return PrintAction(Type::BreakLine);
}

/**
 * ActionPrinter.
 */


ActionPrinter::ActionPrinter(std::vector<PrintAction> &printActions) :
  printActions(printActions) { }

void ActionPrinter::print(const std::string &str) {
  printActions.push_back(PrintAction::Print(str));
}

void ActionPrinter::setColor(const unsigned char r,
                             const unsigned char g,
                             const unsigned char b) {
  printActions.push_back(PrintAction::Color(sf::Color(r, b, g)));

}

void ActionPrinter::breakLine() {
  printActions.push_back(PrintAction::BreakLine());
}

}

/**
 * TextLog.
 */

TextLog::Style::Style(
    const float maxWidth,
    const float maxHeight,
    const float maxHeightCollapsed,
    const float maxLifetime,
    const float maxLifetimeCollapsed,
    const TextFormat &textFormat) :
    maxWidth(maxWidth),
    maxHeight(maxHeight),
    maxHeightCollapsed(maxHeightCollapsed),
    maxLifetime(maxLifetime),
    maxLifetimeCollapsed(maxLifetimeCollapsed),
    textFormat(textFormat) { }

TextLog::TextLog(const Style &style,
                 const sf::Vector2f &pos) :
    style(style), pos(pos) { }

void TextLog::tick(float delta) {
  time += delta;
}

void TextLog::render(Frame &f) {
  f.drawText(pos, [&](TextFrame &tf) {
    for (const auto &pair : printActions) {

    }
  }, style.textFormat);
}

bool TextLog::handle(const sf::Event &event) {
  return Control::handle(event);
}

void TextLog::clear() {
  printActions.clear();
}

void TextLog::print(PrintProcess process) {
  detail::PrintBlock block;
  process(detail::ActionPrinter(block));
  printActions.emplace_back(time, block);
}

}
