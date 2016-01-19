/**
 * Abstract base class for all games that the player can run: offline scrims,
 * tutorials, multiplayer sessions etc.
 */
#ifndef SOLEMNSKY_GAME_H
#define SOLEMNSKY_GAME_H

#include "ui/ui.h"
#include "client/clientstate.h"

class Game : public ui::Control {
protected:
  ClientState *state;

public:
  Game(ClientState *state) : state(state) { }

  /**
   * inFocus is modified by external and internal forces
   */
  bool inFocus;

  std::string name; // description of the game, given by the game itself
};

#endif //SOLEMNSKY_GAME_H
