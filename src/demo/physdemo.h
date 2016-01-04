/**
 * Demonstration of sky::Physics.
 */
#ifndef SOLEMNSKY_PHYSDEMO_H
#define SOLEMNSKY_PHYSDEMO_H

#include "ui/ui.h"
#include "sky/physics.h"

class PhysDemo : public ui::Control {
private:
  b2Body *body;
  sky::Physics physics;

public:
  PhysDemo();

  virtual void tick(float delta) override;
  virtual void render(ui::Frame &f) override;
  virtual void handle(sf::Event event) override;
};

#endif //SOLEMNSKY_PHYSDEMO_H
