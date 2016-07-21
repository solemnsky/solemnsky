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
#include "inputaction.hpp"
#include "clientutil.hpp"

InputAction::InputAction(const sf::Event &event) {
  switch (event.type) {
    case sf::Event::KeyPressed:
    case sf::Event::KeyReleased:
      key.emplace(event.key.code);
      break;
    case sf::Event::JoystickButtonPressed:
    case sf::Event::JoystickButtonReleased:
      joyButton.emplace(event.joystickButton.button);
      break;
    case sf::Event::JoystickMoved:
      joyAxis.emplace(std::make_pair(event.joystickMove.axis, (event.joystickMove.position > 0.0f ? InputAction::AxisDirection::Positive : InputAction::AxisDirection::Negative)));
      break;
    default:
      break;
  }
}

bool InputAction::isMake(const sf::Event &e) const {
  if (key) return e.type == sf::Event::KeyPressed;
  if (joyButton) return e.type == sf::Event::JoystickButtonPressed;
  if (joyAxis) {
    switch (joyAxis->first) {
      case sf::Joystick::Axis::Z:
      case sf::Joystick::Axis::R:
        return e.joystickMove.position > 0.0f;
      default:
        return fabsf(e.joystickMove.position) > 50.0f;
    }
  }

  return false;
}

std::string InputAction::getName() const {
  if (key) {
    return printKey(key.get());
  }
  if (joyButton) {
    return std::string("Button ") + std::to_string(joyButton.get());
  }
  if (joyAxis) {
    return std::string("Axis ") + std::to_string(joyAxis->first) + " " + (joyAxis->second == Negative ? "-" : "+");
  }
  return "<unknown>";
}

bool InputAction::operator==(const InputAction &other) const {
  //Optimize this so we don't have to do a bunch of optional<>::operator bool calls
  if (isKey() && other.isKey()) {
    return key.get() == other.key.get();
  }
  if (isJoyAxis() && other.isJoyAxis()) {
    return joyAxis.get() == other.joyAxis.get();
  }
  if (isJoyButton() && other.isJoyButton()) {
    return joyButton.get() == other.joyButton.get();
  }
  return false;
}

bool InputAction::operator<(const InputAction &other) const {
  //Optimize this so we don't have to do a bunch of optional<>::operator bool calls
  bool thisKey = isKey(), otherKey = other.isKey();
  bool thisJoy = isJoystick(), otherJoy = other.isJoystick();

  //Keys are lower than joystick actions
  if (thisKey && otherJoy) return true;
  if (otherKey && thisJoy) return false;

  //Lowest key wins
  if (thisKey && otherKey) return key.get() < other.key.get();

  //Last case, thisJoy && otherJoy
  bool thisAxis = isJoyAxis(), otherAxis = other.isJoyAxis();
  bool thisButton = isJoyButton(), otherButton = other.isJoyButton();

  //Axis is less than button
  if (thisAxis && otherButton) return true;
  if (thisButton && otherAxis) return false;

  //Lowest button wins
  if (thisButton && otherButton) return joyButton.get() < other.joyButton.get();

  //thisAxis && joyAxis: Negative comes first, then lower axis #
  if (joyAxis->first == other.joyAxis->first) return joyAxis->second == Negative;
  return joyAxis->first < other.joyAxis->first;
}
