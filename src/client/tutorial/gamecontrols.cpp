#include "gamecontrols.h"

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

void GameController::setState(sky::PlaneState &state) {
  state.rotCtrl = Movement::None;
  if (leftCtrl) state.rotCtrl = Movement::Down;
  if (rightCtrl) state.rotCtrl = Movement::Up;
  state.throtCtrl = Movement::None;
  if (upCtrl) state.throtCtrl = Movement::Up;
  if (downCtrl) state.throtCtrl = Movement::Down;
}
