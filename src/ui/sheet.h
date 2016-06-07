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
 * A spritesheet.
 */
#pragma once
#include "frame.h"
#include "util/types.h"

namespace ui {

class SpriteSheet {
 private:
  const SheetLayout &layout;
  const sf::Texture &texture;

 public:
  const int size;

  SpriteSheet() = delete;
  SpriteSheet(const SheetLayout &layout, const sf::Texture &texture);

  void drawIndex(
      Frame &f, const sf::Vector2f &dims, const int index) const;
  void drawIndexAtRoll(
      Frame &f, const sf::Vector2f &dims, const Angle deg) const;

};

}
