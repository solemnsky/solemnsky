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
#include "textlog.hpp"
#include <boost/iterator_adaptors.hpp>

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
    const Time maxLifetimeCollapsed,
    const float fadeStart,
    const int fontSize) :
    maxWidth(maxWidth),
    maxHeight(maxHeight),
    maxHeightCollapsed(maxHeightCollapsed),
    maxLifetimeCollapsed(maxLifetimeCollapsed),
    fadeStart(fadeStart),
    fontSize(fontSize) { }

void TextLog::startNewLine() {
  lines.emplace_back(references.uptime, std::vector<detail::PrintAction>());
  startingNewLine = false;
}

TextLog::TextLog(const AppRefs &references, const Style &style,
                 const sf::Vector2f &pos) :
    Control(references),
    style(style), pos(pos),
    startingNewLine(true),
    textFormat(style.fontSize, 0, ui::HorizontalAlign::Left, ui::VerticalAlign::Bottom),
    collapsed(false) { }

void TextLog::tick(TimeDiff) { }

void TextLog::render(Frame &f) {
  const float maxHeight = collapsed ? style.maxHeightCollapsed : style.maxHeight;

  f.drawText(pos, [&](TextFrame &tf) {
    for (auto iter = lines.rbegin();
         iter < lines.rend();
         iter++) {
      const auto &pair = *iter;
      const Time age = references.timeSince(pair.first);

      if (collapsed) {
        if (age < style.maxLifetimeCollapsed) {
          const double alpha =
              (style.fadeStart == 0) ? 1 :
              clamp(0.0, 1.0, 1 - ((age - style.fadeStart)
                  / (style.maxLifetimeCollapsed - style.fadeStart)));
          f.withAlpha(float(alpha), [&]() {
            for (auto &action : pair.second) action.print(tf);
          });
          tf.breakLine();
        } else {
          break;
        }
      } else {
        for (auto &action : pair.second) action.print(tf);
        tf.breakLine();
      }

      if (-tf.drawOffset.y > maxHeight && maxHeight != 0) break;
    }
  }, textFormat, resources.defaultFont);
}

bool TextLog::handle(const sf::Event &event) {
  return false;
}

void TextLog::print(const std::string &str) {
  if (startingNewLine) startNewLine();
  lines.rbegin()->second.push_back(
      detail::PrintAction::Print(str));
}

void TextLog::setColor(const unsigned char r,
                       const unsigned char g,
                       const unsigned char b) {
  if (startingNewLine) startNewLine();
  lines.rbegin()->second.push_back(
      detail::PrintAction::Color(sf::Color(r, b, g)));
}

void TextLog::breakLine() {
  startingNewLine = true;
}

void TextLog::clear() {
  lines.clear();
}

}
