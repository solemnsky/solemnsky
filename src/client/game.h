/**
 * A page where the player can play a game. This is kind of a big deal for
 * us, because like, it's where people play the game.
 */
#ifndef SOLEMNSKY_GAME_H
#define SOLEMNSKY_GAME_H

#include "ui/ui.h"

class GamePage : public GamePage {
public:
  virtual void tick(float delta) override;
  virtual void render(ui::Frame &f) override;
  virtual void handle(const sf::Event &event) override;
};

#endif //SOLEMNSKY_GAME_H
