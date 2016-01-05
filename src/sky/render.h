/**
 * Essential rendering helpers, for internal use in sky.
 */
#ifndef SOLEMNSKY_GRAPHICS_H_H
#define SOLEMNSKY_GRAPHICS_H_H

#include <base/resources.h>
#include "ui/control.h"
#include "ui/sheet.h"
#include "flight.h"
#include <SFML/Graphics.hpp>
#include "sky.h"

namespace sky {

/**
 * Manages render resources, state, etc, and renders the game of the engine
 * it's linked to.
 */
class RenderMan {
private:
  Sky *sky;
  const ui::SpriteSheet &sheet;

public:
  RenderMan(Sky *sky);

  /**
   * Renders the game.
   * Tries to center on pos.
   */
  void render(ui::Frame &f, sf::Vector2f pos);

  /**
   * Potentially tick forward some animation state / particles.
   */
  void tick(float delta);
};

}

#endif //SOLEMNSKY_GRAPHICS_H_H
