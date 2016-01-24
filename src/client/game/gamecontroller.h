/**
 * Manager for user game control input.
 */
#ifndef SOLEMNSKY_GAMECONTROLLER_H
#define SOLEMNSKY_GAMECONTROLLER_H

#include "client/util/sfmlutil.h"
#include "sky/flight.h"

class GameController {
private:
  void handleKey(bool &state, const sf::Event &event);
public:
  bool leftCtrl{false}, rightCtrl{false},
      upCtrl{false}, downCtrl{false};

  void handle(const sf::Event &event);
  void setState(sky::PlaneVital &state);
};

#endif //SOLEMNSKY_GAMECONTROLLER_H
