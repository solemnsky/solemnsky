/**
 * Demonstrate the basic gameplay mechanics facilities of sky::Engine.
 */
#ifndef SOLEMNSKY_SKYDEMO_H
#define SOLEMNSKY_SKYDEMO_H

#include <ui/sheet.h>
#include "ui/ui.h"
#include "sky/sky.h"

class SkyDemo : public ui::Control {
private:
  const ui::SpriteSheet sheet{Res::PlayerSheet};
  const sf::Texture background;

  Ticker animTicker{0.05f};
  int animVal{0};

  sky::Sky sky{};

public:
  SkyDemo();

  virtual void tick(float delta) override;
  virtual void render(ui::Frame &f) override;
  virtual void handle(sf::Event event) override;
};

#endif //SOLEMNSKY_SKYDEMO_H
