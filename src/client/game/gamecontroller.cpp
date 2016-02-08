#include "gamecontroller.h"

void GameController::handleKey(bool &state, const sf::Event &event) {
  state = event.type == sf::Event::KeyPressed;
}

bool GameController::handle(const sf::Event &event) {
  if (event.type == sf::Event::KeyPressed ||
      event.type == sf::Event::KeyReleased) {
    switch (event.key.code) {
      case sf::Keyboard::J: {
        handleKey(leftCtrl, event);
        return true;
      }
      case sf::Keyboard::L: {
        handleKey(rightCtrl, event);
        return true;
      }
      case sf::Keyboard::I: {
        handleKey(upCtrl, event);
        return true;
      }
      case sf::Keyboard::K: {
        handleKey(downCtrl, event);
        return true;
      }
      default: {
        break;
      }
    }
  }
  return false;
}

void GameController::setState(sky::PlaneVital &state) {
  state.rotCtrl = 0;
  if (leftCtrl) state.rotCtrl = -1;
  if (rightCtrl) state.rotCtrl += 1;
  state.throtCtrl = 0;
  if (upCtrl) state.throtCtrl = 1;
  if (downCtrl) state.throtCtrl = -1;
}
