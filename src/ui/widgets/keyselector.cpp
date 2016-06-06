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
#include "keyselector.h"
#include "util/clientutil.h"

namespace ui {

KeySelector::KeySelector(AppState &appState,
                         const ui::KeySelector::Style &style,
                         const sf::Vector2f &pos) :
    Control(appState),
    button(appState, style, pos, ""),
    capturing(false),
    clickSignal(button.clickSignal) {
  areChildren({&button});
}

void KeySelector::render(ui::Frame &f) {
  Control::render(f);
  const auto body = button.getBody();

  if (capturing) {
    f.drawRect(body, sf::Color::Green);
  }
}

bool KeySelector::handle(const sf::Event &event) {
  if (capturing) {
    if (event.type == sf::Event::KeyPressed) {
      if (event.key.code == sf::Keyboard::Escape) setValue({});
      else setValue(event.key.code);

      capturing = false;
      return true;
    }
  }
  return Control::handle(event);
}

void KeySelector::reset() {
  Control::reset();
  capturing = false;
}

void KeySelector::signalRead() {
  if (clickSignal) {
    setValue({});
    capturing = true;
  }
  Control::signalRead();
}

void KeySelector::signalClear() {
  Control::signalClear();
}

void KeySelector::setValue(const optional<sf::Keyboard::Key> key) {
  value = key;
  if (key) button.text = printKey(key.get());
  else button.text = "<unbound>";
}

optional<sf::Keyboard::Key> KeySelector::getValue() const {
  return value;
}

void KeySelector::setDescription(const optional<std::string> &description) {
  button.description = description;
}

}

