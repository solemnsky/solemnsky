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
#include <util/methods.h>
#include "sheet.h"

namespace ui {

/**
 * SheetLayout.
 */

SheetLayout::SheetLayout(const sf::Vector2i &spriteDimensions,
                         const sf::Vector2i &sheetTiling) :
    spriteDims(spriteDimensions),
    tiling(sheetTiling) { }

/**
 * SpriteSheet.
 */

SpriteSheet::SpriteSheet(const SheetLayout &layout,
                         const sf::Texture &texture) :
    layout(layout),
    texture(texture),
    size(layout.tiling.x * layout.tiling.y) { }

void SpriteSheet::drawIndex(ui::Frame &f, const sf::Vector2f &dims,
                            const int index) const {
  const int
      yShift{index % layout.tiling.y}, xShift{index / layout.tiling.y},
      spriteWidth{layout.spriteDims.x}, spriteHeight{layout.spriteDims.y},
      left{xShift * spriteWidth}, top{yShift * spriteHeight};

  f.withTransform(
      sf::Transform().scale(dims.x / spriteWidth, dims.y / spriteHeight),
      [&] {
        f.drawSprite(texture,
                     {-float(spriteWidth) / 2, -float(spriteHeight) / 2},
                     sf::IntRect(left, top, spriteWidth, spriteHeight));
      });
}

void SpriteSheet::drawIndexAtRoll(ui::Frame &f, const sf::Vector2f &dims,
                                  const Angle deg) const {
  const int fullIndex =
      (deg > 180)
      ? std::floor(Cyclic(0, size * 2, deg * size / 180))
      : std::floor(Cyclic(0, size * 2, (deg * size / 180) - 0.5f));

  bool flipped;
  int realIndex;

  if (fullIndex < size) {
    flipped = false;
    realIndex = fullIndex;
  } else {
    flipped = true;
    realIndex = (size * 2) - fullIndex - 1;
  }

  f.withTransform(sf::Transform().scale(-1, flipped ? -1 : 1), [&]() {
    drawIndex(f, dims, realIndex);
  });
}

}
