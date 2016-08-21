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
#include "button.hpp"
#include "util/methods.hpp"
#include "util/clientutil.hpp"

namespace ui {

Button::Style::Style(const sf::Color &baseColor,
                     const sf::Color &hotColor,
                     const sf::Color &clickedColor,
                     const sf::Color &inactiveColor,
                     const sf::Color &textColor,
                     const sf::Vector2f &dimensions,
                     const float heatRate,
                     const int fontSize) :
    baseColor(baseColor),
    hotColor(hotColor),
    clickedColor(clickedColor),
    inactiveColor(inactiveColor),
    textColor(textColor),
    dimensions(dimensions),
    heatRate(heatRate),
    fontSize(fontSize) { }

sf::FloatRect Button::getBody() {
  return sf::FloatRect(pos, style.dimensions);
}

Button::Button(const AppRefs &references,
               const Style &style,
               const sf::Vector2f &pos,
               const std::string &text) :
    Control(references),
    style(style),
    heat(0),
    inPreClick(false),
    active(true),

    textFormat(style.fontSize, style.dimensions.x, HorizontalAlign::Center, VerticalAlign::Middle),
    descriptionFormat(textFormat),

    pos(pos),
    text(text),

    isHot(false),
    clickSignal(false) {
  descriptionFormat.horizontal = HorizontalAlign::Right;
  descriptionFormat.vertical = VerticalAlign::Bottom;
}

void Button::tick(float delta) {
  if (active) heat += (isHot ? 1 : -1) * delta * style.heatRate;
}

void Button::render(Frame &f) {
  if (description)
    f.drawText(pos + sf::Vector2f(-20, (style.dimensions.y + style.fontSize) / 2.0f),
               description.get(), style.textColor,
               descriptionFormat, resources.defaultFont);

  const auto body = getBody();
  if (!active) {
    f.drawRect(body, style.inactiveColor);
  } else {
    if (!inPreClick)
      f.drawRect(body, mixColors(style.baseColor, style.hotColor, heat));
    else f.drawRect(body, style.clickedColor);
  }

  f.drawText(
      pos + 0.5f * style.dimensions,
      text, style.textColor, textFormat, resources.defaultFont);
}

bool Button::handle(const sf::Event &event) {
  const auto body = getBody();
  if (event.type == sf::Event::MouseMoved) {
    const sf::Vector2f pt(event.mouseMove.x, event.mouseMove.y);
    isHot = body.contains(pt);
    return false;
  }

  if (event.type == sf::Event::MouseButtonPressed or
      event.type == sf::Event::MouseButtonReleased) {
    bool clicking = event.type == sf::Event::MouseButtonPressed;
    const sf::Vector2f pt(event.mouseButton.x, event.mouseButton.y);

    isHot = body.contains(pt);

    if (active) {
      if (clicking) {
        inPreClick = isHot;
      } else {
        inPreClick = false;
        if (isHot) clickSignal = true;
      }
    }
    return isHot;
  }
  return false;
}

void Button::signalClear() {
  clickSignal = false;
}

void Button::setActive(const bool newActive) {
  reset();
  active = newActive;
}

void Button::reset() {
  isHot = false;
  heat = 0;
}

}

