/**
 * Demonstration of sky::Physics.
 */
#ifndef SOLEMNSKY_PHYSDEMO_H
#define SOLEMNSKY_PHYSDEMO_H

#include "ui/ui.h"
#include "sky/physics.h"

class PhysDemo : public ui::Control {
private:
  b2Body *rectBody;
  b2Body *circleBody;
  sky::Physics physics;

public:
  PhysDemo();

  void reset();

  virtual void tick(float delta) override;
  virtual void render(ui::Frame &f) override;
  virtual void handle(sf::Event event) override;
};

#endif //SOLEMNSKY_PHYSDEMO_H
