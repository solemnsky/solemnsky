/**
 * Abstract base class for all games that the player can run: offline scrims,
 * tutorials, multiplayer sessions etc.
 */
#ifndef SOLEMNSKY_GAME_H
#define SOLEMNSKY_GAME_H

#include "ui/ui.h"

class ClientState;

class Game : public ui::Control {
protected:
  ClientState *state;

public:
  Game(ClientState *state) : state(state) { }

  bool inFocus; // set to true by outside forces, false by inside forces
  bool concluded; // modified only by the game itself

  std::string name; // description of the game, given by the game itself
};

#endif //SOLEMNSKY_GAME_H
