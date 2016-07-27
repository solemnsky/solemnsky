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
 * Widget used to select a keyboard key.
 */
#pragma once
#include "ui/control.hpp"
#include "button.hpp"
#include "ui/inputaction.hpp"

namespace ui {

class ActionSelector: public ui::Control {
 private:
  Button button;
  optional<InputAction> value;
  bool capturing;

 public:
  typedef Button::Style Style;

  ActionSelector(const AppRefs &references,
              const Style &style, const sf::Vector2f &pos);

  // Control impl.
  void render(Frame &f) override;
  bool handle(const sf::Event &event) override;
  void reset() override;
  void signalRead() override;
  void signalClear() override;

  // Custom API.
  void setValue(const optional<InputAction> action);
  optional<InputAction> getValue() const;

  void setDescription(const optional<std::string> &description);

  bool &clickSignal;
};

}
