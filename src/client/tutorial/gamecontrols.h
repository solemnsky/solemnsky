/**
 * TODO: refactor this along with key bindings
 */
#pragma once
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
