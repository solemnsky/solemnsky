/**
 * A spritesheet.
 */
#pragma once
#include "frame.h"
#include "util/types.h"

namespace ui {
class SpriteSheet {
private:
  const ResRecord record;
  const sf::Vector2f tileDim;
  const sf::Texture &sheet;
  const ResID res;

public:
  const int count;

  SpriteSheet() = delete;
  SpriteSheet(ResID resource);

  void drawIndex(
      ui::Frame &f, const sf::Vector2f &dims, const int index) const;
  void drawIndexAtRoll(
      ui::Frame &f, const sf::Vector2f &dims, const Angle deg) const;
};
}
