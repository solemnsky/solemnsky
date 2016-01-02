/**
 * Demonstrate the basic gameplay mechanics facilities of sky::Engine.
 */
#ifndef SOLEMNSKY_ENGINEDEMO_H
#define SOLEMNSKY_ENGINEDEMO_H

#include "ui/control.h"
#include "sky/sky.h"

class EngineDemo : public ui::Control {
private:
  sky::Sky engine;
public:
  virtual void tick(float delta) override;
  virtual void render(ui::Frame &f) override;
  virtual void handle(sf::Event event) override;
};

#endif //SOLEMNSKY_ENGINEDEMO_H
