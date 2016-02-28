/**
 * User input actions in the game.
 */
#ifndef SOLEMNSKY_GAMECONTROLLER_H
#define SOLEMNSKY_GAMECONTROLLER_H

#include "client/util/sfmlutil.h"
#include "sky/flight.h"

/**
 * A
 */
enum class GameControl {

};

class GameController {
private:
  void handleKey(bool &state, const sf::Event &event);
public:
  bool leftCtrl{false}, rightCtrl{false},
      upCtrl{false}, downCtrl{false};

  bool handle(const sf::Event &event);
  void setState(sky::PlaneState &state);
};

#endif //SOLEMNSKY_GAMECONTROLLER_H
