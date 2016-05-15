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

SpriteSheet::SpriteSheet(ResID resource) :
    record(recordOf(resource)),
    tileDim{(float) record.tileX, (float) record.tileY},
    sheet(textureOf(resource)),
    res(resource),
    count(record.countX * record.countY) { }

void SpriteSheet::drawIndex(ui::Frame &f, const sf::Vector2f &dims,
                            const int index) const {
  const int yShift(index % record.countY), xShift(index / record.countY);
  const float spriteWidth(record.tileX), spriteHeight(record.tileY);
  const int left(xShift * record.tileX), top(yShift * record.tileY);

  f.pushTransform(
      sf::Transform().scale(dims.x / spriteWidth, dims.y / spriteHeight));
  f.drawSprite(sheet, {-spriteWidth / 2, -spriteHeight / 2},
               sf::IntRect(left, top, record.tileX, record.tileY));
  f.popTransform();
}

void SpriteSheet::drawIndexAtRoll(ui::Frame &f, const sf::Vector2f &dims,
                                  const Angle deg) const {
  const int fullIndex =
      (deg > 180)
      ? std::floor(Cyclic(0, count * 2, deg * count / 180))
      : std::floor(Cyclic(0, count * 2, (deg * count / 180) - 0.5f));

  bool flipped;
  int realIndex;

  if (fullIndex < count) {
    flipped = false;
    realIndex = fullIndex;
  } else {
    flipped = true;
    realIndex = (count * 2) - fullIndex - 1;
  }

  f.withTransform(sf::Transform().scale(-1, flipped ? -1 : 1), [&]() {
    drawIndex(f, dims, realIndex);
  });
}

}
