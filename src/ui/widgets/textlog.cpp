#include "textlog.h"

namespace ui {

namespace detail {

/**
 * PrintAction.
 */

PrintAction::PrintAction(const PrintAction::Type type) :
    type(type) { }

void PrintAction::print(Printer &p) const {
  switch (type) {
    case Type::Color: {
      p.setColor(color->r, color->g, color->b);
      break;
    }
    case Type::Print: {
      p.print(string.get());
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
    const int fontSize) :
    maxWidth(maxWidth),
    maxHeight(maxHeight),
    maxHeightCollapsed(maxHeightCollapsed),
    maxLifetime(maxLifetime),
    maxLifetimeCollapsed(maxLifetimeCollapsed),
    fontSize(fontSize) { }

void TextLog::startNewLine() {
  lines.emplace_back(appState.time, std::vector<detail::PrintAction>());
  startingNewLine = false;
}

TextLog::TextLog(AppState &appState, const Style &style,
                 const sf::Vector2f &pos) :
    Control(appState),
    style(style), pos(pos),
    startingNewLine(true),
    textFormat(style.fontSize, 0, ui::HorizontalAlign::Left,
               ui::VerticalAlign::Bottom, ResID::Font) {

}

void TextLog::tick(float delta) { }

void TextLog::render(Frame &f) {
  const float
      maxHeight = collapsed ? style.maxHeightCollapsed : style.maxHeight,
      maxLifetime = collapsed ? style.maxLifetimeCollapsed : style.maxLifetime;

  f.drawText(pos, [&](TextFrame &tf) {
    for (const auto &pair : lines) {
      const double age = appState.timeSince(pair.first);
      if (age < maxLifetime || maxLifetime == 0) {
        const double alpha =
            (style.fadeStart == 0) ? 1 :
            clamp(0.0, 1.0, (maxLifetime - age) / style.fadeStart);
        f.withAlpha(float(alpha), [&]() {
          for (auto &action : pair.second) action.print(tf);
        });
        tf.breakLine();
      }

      if (tf.drawOffset.y > maxHeight && maxHeight != 0) return;
    }
  }, textFormat);
}

bool TextLog::handle(const sf::Event &event) {
  return false;
}

void TextLog::print(const std::string &str) {
  if (startingNewLine) startNewLine();
  lines.end()->second.push_back(detail::PrintAction::Print(str));
}

void TextLog::setColor(const unsigned char r,
                       const unsigned char g,
                       const unsigned char b) {
  if (startingNewLine) startNewLine();
  lines.end()->second.push_back(
      detail::PrintAction::Color(sf::Color(r, b, g)));
}

void TextLog::breakLine() {
  startingNewLine = true;
}

void TextLog::clear() {
  lines.clear();
}

}
