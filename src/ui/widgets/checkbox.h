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
 * The ubiquitous checkbox.
 */
#pragma once
#include "ui/control.h"
#include "button.h"

namespace ui {

class Checkbox: public ui::Control {
 private:
  Button button;
  bool value;

 public:
  typedef Button::Style Style;

  Checkbox() = delete;
  Checkbox(const AppState &appState,
           const Style &style,
           const sf::Vector2f &pos);

  void signalRead() override;
  void signalClear() override;

  // User API.
  void setValue(const bool newValue);
  bool getValue() const;

  void setDescription(const optional<std::string> &description);

  bool &clickSignal;
};

}
