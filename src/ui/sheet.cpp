#include "sheet.h"

namespace ui {
void SpriteSheet::drawIndex(ui::Frame &f, const sf::Vector2f pos,
                            const sf::Vector2f dims, const int index) const {
  const int yShift(index % record.countY), xShift(index / record.countY);
  const float spriteWidth(record.tileX), spriteHeight(record.tileY);
  const int left(xShift * record.tileX), top(yShift * record.tileY);

  f.pushTransform(sf::Transform().translate(pos).
      scale(dims.x / spriteWidth, dims.y / spriteHeight));
  f.drawSprite(texture, {-spriteWidth / 2, -spriteHeight / 2},
               sf::IntRect(left, top, record.tileX, record.tileY));
  f.popTransform();
}

}
