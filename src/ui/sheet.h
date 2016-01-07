/**
 * Abstraction over our much beloved spritesheets (for planes, weapons etc).
 */
#ifndef SOLEMNSKY_SHEET_H
#define SOLEMNSKY_SHEET_H

#include "frame.h"
#include "base/resources.h"
#include "base/util.h"

namespace ui {
class SpriteSheet {
private:
  const sf::Vector2f tileDim;
  const sf::Texture &sheet;
  const ResRecord record;
  const Res res;

public:
  const int count;

  SpriteSheet() = delete;

  SpriteSheet(Res resource) :
      record(recordOf(resource)),
      res(res),
      sheet(textureOf(resource)),
      tileDim{(float) record.tileX, (float) record.tileY},
      count(record.countX * record.countY) {
    appLog(LogType::Info, record.path);
  }

  void drawIndex(ui::Frame &f, const sf::Vector2f dims,
                 const int index) const;
};
}

#endif //SOLEMNSKY_SHEET_H
