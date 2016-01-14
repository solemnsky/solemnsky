/**
 * Demonstrate the basic gameplay mechanics facilities of sky::Engine.
 */
#ifndef SOLEMNSKY_SKYDEMO_H
#define SOLEMNSKY_SKYDEMO_H

#include <ui/sheet.h>
#include "ui/ui.h"
#include "sky/sky.h"
#include "sky/render.h"

class ControlState {
private:
  optional<char> charFromEvent(const sf::Event &event);
  void handleKey(bool &state, const sf::Event &event);
public:
  bool leftCtrl{false}, rightCtrl{false},
      upCtrl{false}, downCtrl{false};

  void handle(const sf::Event &event);
  void setState(sky::PlaneState &state);
};

class SkyDemo : public ui::Control {
private:
  ControlState ctrlState;

  sky::RenderMan renderMan;

  Cooldown animTicker;
  int animVal;

  sky::Sky sky;
  sky::Plane *plane;

public:
  SkyDemo();

  virtual void tick(float delta) override;
  virtual void render(ui::Frame &f) override;
  virtual void handle(const sf::Event &event) override;
};

#endif //SOLEMNSKY_SKYDEMO_H
