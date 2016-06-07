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
 * Widget that displays a log created through the Printer interface.
 */
#pragma once
#include "ui/control.h"
#include "util/printer.h"

namespace ui {

namespace detail {

struct PrintAction {
 private:
  enum class Type {
    Color, Print
  } type;

  PrintAction(const Type type);
 public:
  optional<sf::Color> color;
  optional<std::string> string;

  void print(Printer &) const;

  static PrintAction Color(const sf::Color &color);
  static PrintAction Print(const std::string &string);
};

using LogLine = std::vector<PrintAction>;

}

class TextLog: public Control, public Printer {
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

  void startNewLine();
  bool startingNewLine;
  std::vector<std::pair<double, detail::LogLine>> lines;

  TextFormat textFormat; // derived from style

 public:
  TextLog
      (const AppState &appState, const Style &style, const sf::Vector2f &pos);

  // Control implementation
  void tick(float delta) override final;
  void render(Frame &f) override final;
  bool handle(const sf::Event &event) override final;

  // Printer implementation
  void print(const std::string &str) override final;
  void setColor(const unsigned char r,
                const unsigned char g,
                const unsigned char b) override final;
  void breakLine() override final;

  void clear();

  bool collapsed;
};

}

