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
#include "checkbox.h"
#include "util/methods.h"

namespace ui {

Checkbox::Checkbox(const AppState &appState,
                   const ui::Button::Style &style,
                   const sf::Vector2f &pos) :
    Control(appState),
    button(appState, style, pos, ""),
    value(false),
    clickSignal(button.clickSignal) {
  areChildren({&button});
}

void Checkbox::signalRead() {
  if (button.clickSignal) {
    setValue(!value);
  }
}

void Checkbox::signalClear() {
  button.signalClear();
}

void Checkbox::setValue(const bool newValue) {
  value = newValue;
  button.text = newValue ? "x" : " ";
}

bool Checkbox::getValue() const {
  return value;
}

void Checkbox::setDescription(const optional<std::string> &description) {
  button.description = description;
}

}
