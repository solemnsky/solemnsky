/**
 * The
 */
#ifndef SOLEMNSKY_TUTORIAL_H
#define SOLEMNSKY_TUTORIAL_H

#include "game.h"
#include "sky/sky.h"
#include "sky/client/render.h"
#include "gamecontroller.h"

class Tutorial : public Game {
private:
  sky::Sky sky;
  sky::Render renderSystem;
  GameController controller;

  sky::Plane *plane;

public:
  Tutorial(ClientState *state);

  virtual void tick(float delta) override;
  virtual void render(ui::Frame &f) override;
  virtual void handle(const sf::Event &event) override;
};

#endif //SOLEMNSKY_TUTORIAL_H
