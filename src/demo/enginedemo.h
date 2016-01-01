/**
 * Demonstrate the basic gameplay mechanics facilities of sky::Engine.
 */
#ifndef SOLEMNSKY_ENGINEDEMO_H
#define SOLEMNSKY_ENGINEDEMO_H

#include "base/control.h"
#include "engine/engine.h"

class EngineDemo : public Control {
private:
  sky::Engine engine;
public:
  virtual void tick(float delta) override;
  virtual void render(Frame &f) override;
  virtual void handle(sf::Event event) override;
};

#endif //SOLEMNSKY_ENGINEDEMO_H
