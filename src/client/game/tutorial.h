/**
 * The
 */
#ifndef SOLEMNSKY_TUTORIAL_H
#define SOLEMNSKY_TUTORIAL_H

#include "game.h"

class Tutorial : public Game {
public:
  Tutorial(ClientState *state);

  virtual void tick(float delta) override;
  virtual void render(ui::Frame &f) override;
  virtual void handle(const sf::Event &event) override;
};

#endif //SOLEMNSKY_TUTORIAL_H
