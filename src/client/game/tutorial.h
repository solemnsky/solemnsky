/**
 * The
 */
#ifndef SOLEMNSKY_GAME_H
#define SOLEMNSKY_GAME_H

#include "game.h"

class Tutorial : public Game {
private:

public:
  virtual void tick(float delta) override;
  virtual void render(ui::Frame &f) override;
  virtual void handle(const sf::Event &event) override;
};

#endif //SOLEMNSKY_GAME_H
