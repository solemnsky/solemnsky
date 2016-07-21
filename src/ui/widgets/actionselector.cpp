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
#include "actionselector.hpp"
#include "util/clientutil.hpp"

namespace ui {

ActionSelector::ActionSelector(const AppRefs &references,
                         const ui::ActionSelector::Style &style,
                         const sf::Vector2f &pos) :
    Control(references),
    button(references, style, pos, ""),
    capturing(false),
    clickSignal(button.clickSignal) {
  areChildren({&button});
}

void ActionSelector::render(ui::Frame &f) {
  Control::render(f);
  const auto body = button.getBody();

  if (capturing) {
    f.drawRect(body, sf::Color::Green);
  }
}

bool ActionSelector::handle(const sf::Event &event) {
  if (capturing) {
    switch (event.type) {
      case sf::Event::KeyPressed:
        if (event.key.code == sf::Keyboard::Escape) setValue({});
        else setValue(InputAction(event.key.code));

        capturing = false;
        return true;
      case sf::Event::JoystickButtonPressed:
        setValue(InputAction(event.joystickButton.button));

        capturing = false;
        return true;
      case sf::Event::JoystickMoved:
        if (fabsf(event.joystickMove.position) > 50.f) {
          setValue(InputAction(event.joystickMove.axis, (event.joystickMove.position > 0.0f ? InputAction::AxisDirection::Positive : InputAction::AxisDirection::Negative)));

          capturing = false;
          return true;
        }
      default:
        break;
    }
  }
  return Control::handle(event);
}

void ActionSelector::reset() {
  Control::reset();
  capturing = false;
}

void ActionSelector::signalRead() {
  if (clickSignal) {
    setValue({});
    capturing = true;
  }
  Control::signalRead();
}

void ActionSelector::signalClear() {
  Control::signalClear();
}

void ActionSelector::setValue(const optional<InputAction> action) {
  value = action;
  if (action) button.text = action->getName();
  else button.text = "<unbound>";
}

optional<InputAction> ActionSelector::getValue() const {
  return value;
}

void ActionSelector::setDescription(const optional<std::string> &description) {
  button.description = description;
}

}

