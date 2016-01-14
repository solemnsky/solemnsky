/**
 * Demonstration of sky::Physics.
 */
#ifndef SOLEMNSKY_PHYSDEMO_H
#define SOLEMNSKY_PHYSDEMO_H

#include "ui/ui.h"
#include "sky/physics.h"

class PhysDemo : public ui::Control {
private:
  sky::Physics physics;
  std::vector<b2Body *> bodies;
  bool marchingLeft;
  bool rising;

public:
  PhysDemo();

  void reset();
  virtual void tick(float delta) override;
  virtual void render(ui::Frame &f) override;
  virtual void handle(const sf::Event &event) override;
};

#endif //SOLEMNSKY_PHYSDEMO_H
