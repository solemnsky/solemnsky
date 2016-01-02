/**
 * Essential rendering helpers.
 */
#ifndef SOLEMNSKY_GRAPHICS_H_H
#define SOLEMNSKY_GRAPHICS_H_H

#include <base/resources.h>
#include "ui/control.h"

namespace sky {
enum class SpriteSheetId {
  BasicPlayer
};

/**
 * Represents a sprite
 */
class SpriteSheet {
private:
  const std::string filepath;

  const sf::Vector2i spriteSize;
  const sf::Vector2i spriteLayout;
public:
  const int spriteCount;

  SpriteSheet(std::string filepath,
              sf::Vector2i spriteSize,
              sf::Vector2i spriteLayout) :
      spriteSize(spriteSize),
      spriteLayout(spriteLayout),
      filepath(filepath),
      spriteCount(spriteLayout.x * spriteLayout.y) { }

  // a number from 1 to spriteCount
  void drawAt(ui::Frame &f, const sf::Vector2f pos,
              const sf::Vector2f dims, const int index) const;
};

/**
 * Static record of sprite sheet data that we have in the builtin assets.
 */
static const std::vector<SpriteSheet> spriteSheets{
    {getIdPath("render-3d/test_1/player_200.png"), {200, 200}, {2, 15}}
};

/**
 * Indices of various player sheets in 'spriteSheets'.
 */
enum class PlayerSheets {

};

}

#endif //SOLEMNSKY_GRAPHICS_H_H
