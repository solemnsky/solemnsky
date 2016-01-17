#include "client.h"

optional<char> ControlState::charFromEvent(const sf::Event &event) {
  if (event.type == sf::Event::KeyPressed ||
      event.type == sf::Event::KeyReleased) {
  }
}

void ControlState::handleKey(bool &state, const sf::Event &event) {
  state = event.type == sf::Event::KeyPressed;
}

void ControlState::handle(const sf::Event &event) {
  if (event.type == sf::Event::KeyPressed ||
      event.type == sf::Event::KeyReleased) {
    switch (event.key.code) {
      case sf::Keyboard::J: {
        handleKey(leftCtrl, event);
        break;
      }
      case sf::Keyboard::L: {
        handleKey(rightCtrl, event);
        break;
      }
      case sf::Keyboard::I: {
        handleKey(upCtrl, event);
        break;
      }
      case sf::Keyboard::K: {
        handleKey(downCtrl, event);
        break;
      }
      default: {
        break;
      }
    }
  }
}

void ControlState::setState(sky::PlaneState &state) {
  state.rotCtrl = 0;
  if (leftCtrl) state.rotCtrl = -1;
  if (rightCtrl) state.rotCtrl += 1;
  state.throtCtrl = 0;
  if (upCtrl) state.throtCtrl = 1;
  if (downCtrl) state.throtCtrl = -1;
}

Client::Client() :
    sky({3200, 900}),
    renderSystem(&sky) {
  sky.linkSystem(&renderSystem);
  plane = sky.joinPlane(0, sky::PlaneTuning());
}

void Client::tick(float delta) {
  sky.tick(delta);
}

void Client::render(ui::Frame &f) {
  if (plane->state) {
    renderSystem.render(f, plane->state->pos);
  } else {
    renderSystem.render(f, {0, 0});
  }
}

void Client::handle(const sf::Event &event) {
  if (event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::F)
      sky.spawnPlane(0, {200, 200}, 0, sky::PlaneTuning());
  }

  ctrlState.handle(event);
  if (plane->state) ctrlState.setState(*plane->state);
}