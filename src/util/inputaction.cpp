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
