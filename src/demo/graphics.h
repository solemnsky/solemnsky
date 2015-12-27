/**
 * Make sure graphics work; we shan't have any embarrassments, yes?
 */
#ifndef SOLEMNSKY_GRAPHICS_H
#define SOLEMNSKY_GRAPHICS_H

#include "base/control.h"

class GraphicsDemo : public Control {
  sf::Vector2f mousePos{};
  float time = 0;

  void renderPlanet(Frame &f, sf::Vector2f center);
  void renderSystem(Frame &f);

public:
  void tick(float delta) override;
  void render(Frame &f) override;
  void handle(sf::Event event) override;
  optional<Control &> passOn() override;
};

#endif //SOLEMNSKY_GRAPHICS_H

