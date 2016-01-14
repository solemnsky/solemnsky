/**
 * Render state / routine containment, detached and above sky::Sky.
 */
#ifndef SOLEMNSKY_GRAPHICS_H
#define SOLEMNSKY_GRAPHICS_H

#include <base/resources.h>
#include "ui/control.h"
#include "ui/sheet.h"
#include "flight.h"
#include <SFML/Graphics.hpp>
#include "sky.h"

namespace sky {

class RenderMan {
private:
  Sky *sky;
  const ui::SpriteSheet sheet;

  /**
   * Render submethods.
   */
  float findView(const float viewWidth,
                 const float totalWidth,
                 const float viewTarget) const;
  void renderPlane(ui::Frame &f, const int pid, Plane &plane);

public:
  RenderMan(Sky *sky);

  /**
   * Renders the game.
   * Tries to center on pos.
   */
  void render(ui::Frame &f, const sf::Vector2f &pos);

  /**
   * Tick forward animation state.
   */
  void tick(float delta);
};

}

#endif //SOLEMNSKY_GRAPHICS_H
