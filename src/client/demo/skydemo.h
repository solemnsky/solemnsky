/**
 * Demonstrate the basic gameplay mechanics facilities of sky::Engine.
 */
#ifndef SOLEMNSKY_SKYDEMO_H
#define SOLEMNSKY_SKYDEMO_H

#include "ui/control.h"

class SkyDemo : public ui::Control {
public:
  virtual void tick(float delta);
  virtual void render(ui::Frame &f);
  virtual void handle(const sf::Event &event);
};


#endif //SOLEMNSKY_SKYDEMO_H
