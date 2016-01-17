#include "sheet.h"
#include "base/sysutil.h"

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
  const int fullIndex = std::floor(
      Cyclic(0, count * 2, (deg * count / 180) - 0.5f)
  );

  bool flipped;
  int realIndex;

  appLog(LogType::Debug, std::to_string(deg));
  if (fullIndex < count) {
    flipped = false;
    realIndex = fullIndex;
  } else {
    flipped = true;
    realIndex = (count * 2) - fullIndex - 1;
  }

  f.withTransform(sf::Transform().scale(1, flipped ? -1 : 1), [&]() {
    drawIndex(f, dims, realIndex);
  });
}
}
