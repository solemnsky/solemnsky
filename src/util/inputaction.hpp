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

#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "util/types.hpp"

struct InputAction {
  enum AxisDirection {
    Positive,
    Negative
  };

  typedef unsigned int JoystickButton;

  optional<sf::Keyboard::Key> key;
  optional<std::pair<sf::Joystick::Axis, AxisDirection>> joyAxis;
  optional<JoystickButton> joyButton;

  InputAction() : key(), joyAxis(), joyButton() {

  }

  InputAction(const sf::Keyboard::Key &key) : key(key), joyAxis(), joyButton() {

  }
  InputAction(const sf::Joystick::Axis &axis, const AxisDirection &direction) : key(), joyAxis(std::make_pair(axis, direction)), joyButton() {

  }
  InputAction(const JoystickButton &button) : key(), joyAxis(), joyButton(button) {

  }
  InputAction(const sf::Event &e);

  inline bool isKey() const { return !!key; }
  inline bool isJoyAxis() const { return !!joyAxis; }
  inline bool isJoyButton() const { return !!joyButton; }
  inline bool isJoystick() const { return isJoyAxis() || isJoyButton(); }

  inline operator bool() const { return isKey() || isJoystick(); }

  //If an event (that matches this action) is make
  bool isMake(const sf::Event &e) const;

  //Comparison, for std::sort and friends
  bool operator==(const InputAction &other) const;
  bool operator==(InputAction *other) const {
    return operator==(*other);
  }
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

  //Cereal really does not want to find the template optional save/load so I have to remake this
  template<typename Archive>
  void save(Archive &ar) const {
    //TODO: There has got to be a better way to do this
    ar(isKey());
    if (isKey()) {
      ar(key.get());
    }
    ar(isJoyAxis());
    if (isJoyAxis()) {
      ar(joyAxis.get());
    }
    ar(isJoyButton());
    if (isJoyButton()) {
      ar(joyButton.get());
    }
  }

  template<typename Archive>
  void load(Archive &ar) {
    bool _key, _axis, _button;
    ar(_key);
    if (_key) {
      sf::Keyboard::Key k;
      ar(k);
      key.emplace(k);
    }
    ar(_axis);
    if (_axis) {
      std::pair<sf::Joystick::Axis, AxisDirection> p;
      ar(p);
      joyAxis.emplace(p);
    }
    ar(_button);
    if (_button) {
      unsigned int b;
      ar(b);
      joyButton.emplace(b);
    }
  }

  //For visual prettiness
  std::string getName() const;
};
