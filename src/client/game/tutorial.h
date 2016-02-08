/**
 * The
 */
#ifndef SOLEMNSKY_TUTORIAL_H
#define SOLEMNSKY_TUTORIAL_H

#include "client/elements/elements.h"
#include "sky/sky.h"
#include "client/subsystem/render.h"
#include "gamecontroller.h"

class Tutorial : public Game {
private:
  ui::Button quitButton;

  sky::Sky sky;
  sky::Render renderSystem;
  GameController controller;

  sky::Plane *plane;

public:
  Tutorial(ClientShared &state);

  /**
   * Control interface.
   */
  virtual void tick(float delta) override;
  virtual void render(ui::Frame &f) override;
  virtual bool handle(const sf::Event &event) override;

  void signalRead() override;
  void signalClear() override;
};

#endif //SOLEMNSKY_TUTORIAL_H
