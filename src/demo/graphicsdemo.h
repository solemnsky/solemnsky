/**
 * Make sure graphics work; we shan't have any embarrassments, yes?
 */
#ifndef SOLEMNSKY_GRAPHICSDEMO_H
#define SOLEMNSKY_GRAPHICSDEMO_H

#include "ui/ui.h"

class GraphicsDemo : public ui::Control {
  sf::Vector2f mousePos{};
  float time = 0;

  void renderPlanet(ui::Frame &f, sf::Vector2f center);
  void renderSystem(ui::Frame &f);

public:
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  void handle(sf::Event event) override;
};

#endif //SOLEMNSKY_GRAPHICSDEMO_H

