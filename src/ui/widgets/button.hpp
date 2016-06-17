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
 * The ubiquitous button.
 */
#pragma once
#include "ui/control.h"
#include "util/types.h"

namespace ui {

/**
 * A rectangular button.
 */
class Button: public Control {
 public:
  struct Style {
    sf::Color baseColor, hotColor, clickedColor, inactiveColor, textColor;
    sf::Vector2f dimensions;
    float heatRate;
    int fontSize;

    Style() = delete;
    Style(const sf::Color &baseColor,
          const sf::Color &hotColor,
          const sf::Color &clickedColor,
          const sf::Color &inactiveColor,
          const sf::Color &textColor,
          const sf::Vector2f &dimensions,
          const float heatRate,
          const int fontSize);
  } style;

 private:
  Clamped heat;
  bool inPreClick, active;

  sf::FloatRect getBody();

  friend class KeySelector;
  TextFormat textFormat, descriptionFormat; // deduced from the style

 public:
  Button(const AppRefs &references,
         const Style &style,
         const sf::Vector2f &pos,
         const std::string &text);

  sf::Vector2f pos;
  std::string text; // what the button has to say
  
  // Control impl.
  void tick(float delta) override;
  void render(Frame &f) override;
  bool handle(const sf::Event &event) override;
  void signalClear() override;
  void reset() override;


  // User API.
  optional<std::string> description;

  void setActive(const bool active);

  bool isHot;
  bool clickSignal;
};

}
