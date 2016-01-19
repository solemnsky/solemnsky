/**
 * Abstract base class for all games that the player can run: offline scrims,
 * tutorials, multiplayer sessions etc. When the game is in session it's
 * treated the same by the rest of the client UI system.
 */
#ifndef SOLEMNSKY_GAME_H
#define SOLEMNSKY_GAME_H

#include "ui/ui.h"

class Game : public ui::Control {
public:
  ui::Signal<bool> gameOver; // signal that the game is now over, with
  // whether the player won or lost
  bool inGame; // if we're inGame
  std::string name; // description of the game
};

#endif //SOLEMNSKY_GAME_H
