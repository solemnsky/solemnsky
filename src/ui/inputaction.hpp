/**
 * solemnsky: the open-source multiplayer competitive 2D plane game
 * Copyright (C) 2016  Chris Gadzinski
 * Copyright (C) 2016  Glenn Smith
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

#pragma once
#include <SFML/Window.hpp>
#include "util/types.hpp"

namespace ui {

/**
 * Input Actions that signifiy either a key, joystick axis (in one direction), or joystick button.
 * Actions don't have a make/break field, but you can test an event against an action to
 * see if that event would be a make event for the action.
 */
struct InputAction {
    enum AxisDirection {
      Positive,
      Negative
    };

  typedef unsigned int JoystickButton;

  optional<sf::Keyboard::Key> key;
  optional<std::pair<sf::Joystick::Axis, AxisDirection>> joyAxis;
  optional<JoystickButton> joyButton;

  //Get the (most relevant) action that an event corresponds with
  static InputAction actionForEvent(const sf::Event &event);
  //Get all the actions that an event corresponds with (eg both +/- joystick axes)
  static std::vector<InputAction> actionsForEvent(const sf::Event &event);

  InputAction() = default;

  InputAction(const sf::Keyboard::Key &key);
  InputAction(const sf::Joystick::Axis &axis, const AxisDirection &direction);
  InputAction(const JoystickButton &button);

  //Convenience
  inline bool isKey() const { return !!key; }
  inline bool isJoyAxis() const { return !!joyAxis; }
  inline bool isJoyButton() const { return !!joyButton; }
  //Joystick actions are either axes or buttons
  inline bool isJoystick() const { return isJoyAxis() || isJoyButton(); }

  //Is the action anything at all?
  inline operator bool() const { return isKey() || isJoystick(); }

  //If an event (that matches this action) is make
  bool isMake(const sf::Event &e) const;

  //Comparison, for std::sort and friends
  bool operator==(const InputAction &other) const;
  inline bool operator!=(const InputAction &other) const {
    return !operator==(other);
  }
  bool operator<(const InputAction &other) const;
  inline bool operator>=(const InputAction &other) const {
    return !operator<(other);
  }
  inline bool operator>(const InputAction &other) const {
    return !operator==(other) && operator>=(other);
  }
  inline bool operator<=(const InputAction &other) const {
    return !operator>(other);
  }

  //Pointers in case someone is crazy
  bool operator==(InputAction *other) const { return operator==(*other); }
  bool operator<=(InputAction *other) const { return operator<=(*other); }
  bool operator>=(InputAction *other) const { return operator>=(*other); }
  bool operator<(InputAction *other) const { return operator<(*other); }
  bool operator>(InputAction *other) const { return operator>(*other); }

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(cereal::make_nvp("key", key),
       cereal::make_nvp("joyAxis", joyAxis),
       cereal::make_nvp("isJoyButton", joyButton));
  }

  //For visual prettiness
  std::string getName() const;
};

}
