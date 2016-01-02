/**
 * Demonstrate the basic gameplay mechanics facilities of sky::Engine.
 */
#ifndef SOLEMNSKY_SKYDEMO_H
#define SOLEMNSKY_SKYDEMO_H

#include "ui/ui.h"
#include "sky/sky.h"

class SkyDemo : public ui::Control {
private:
  ui::Button button{{100, 50}, "click to go back"};

  const sf::Texture background;

  sky::Sky sky;

public:
  SkyDemo();

  virtual void tick(float delta) override;
  virtual void render(ui::Frame &f) override;
  virtual void handle(sf::Event event) override;
};

#endif //SOLEMNSKY_SKYDEMO_H
