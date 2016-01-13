#include "sheet.h"
#include "base/util.h"

namespace ui {
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
  if (deg < 180) {
    const int id = (const int) std::floor(count * deg / 180);
    drawIndex(f, dims, id);
  } else {
    f.withTransform(sf::Transform().scale(1, -1), [&]() {
      const int id = (const int) std::floor(count * (360 - deg) / 180);
      drawIndex(f, dims, id);
    });
  }
  // wow much elegant
}
}
