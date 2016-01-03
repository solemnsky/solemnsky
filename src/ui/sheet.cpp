#include "sheet.h"

namespace ui {
void SpriteSheet::drawAt(ui::Frame &f, const sf::Vector2f pos,
                         const sf::Vector2f dims, const int index) const {
  const int yShift = index / record.countX;
  const int xShift = index % record.countX;
  const float spriteWidth(record.tileX), spriteHeight(record.tileY);

  f.pushTransform(sf::Transform().scale(dims.x / spriteWidth, dims.y / spriteHeight)
                      .translate(pos));
//  f.drawSprite(texture, {-spriteWidth / 2, -spriteHeight / 2},
//               sf::IntRect(xShift * record.tileX, yShift * record.tileY,
//                           record.tileX, record.tileY));
  f.drawSprite(texture, {0, 0}, {0, 0, 100, 100});
  f.popTransform();
}
}
