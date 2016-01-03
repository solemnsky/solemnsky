/**
 * Abstraction over our much beloved spritesheets (for planes, weapons etc).
 */
#ifndef SOLEMNSKY_SHEET_H
#define SOLEMNSKY_SHEET_H

#include "frame.h"
#include "base/resources.h"

namespace ui {
class SpriteSheet {
private:
  sf::Texture texture;
  const sf::Vector2f tileDim;
  const ResRecord record;

public:
  const int count;

  SpriteSheet() = delete;

  SpriteSheet(Res resource) :
      record(recordOf(resource)),
      tileDim{(float) record.tileX, (float) record.tileY},
      count(record.countX * record.countY) {
    texture.loadFromFile(filepathTo(resource));
  }

  void drawAt(ui::Frame &f, const sf::Vector2f pos,
              const sf::Vector2f dims, const int index) const;
};
}

#endif //SOLEMNSKY_SHEET_H
