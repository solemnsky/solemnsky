#include "skydemo.h"
#include "demo.h"
#include "base/resources.h"
#include "ui/sheet.h"

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
      case sf::Keyboard::Key::J: {
        handleKey(leftCtrl, event);
        break;
      }
      case sf::Keyboard::Key::L: {
        handleKey(rightCtrl, event);
        break;
      }
      case sf::Keyboard::Key::I: {
        handleKey(upCtrl, event);
        break;
      }
      case sf::Keyboard::Key::K: {
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

SkyDemo::SkyDemo() :
    sky({3200, 900}),
    animTicker{0.05f},
    animVal{0} {
  sky.enableRender();
  plane = sky.joinPlane(0, sky::PlaneTuning());
}

void SkyDemo::tick(float delta) {
  sky.tick(delta);
}

void SkyDemo::render(ui::Frame &f) {
  if (plane->state) {
    sky.render(f, plane->state->pos);
  } else {
    sky.render(f, {0, 0});
  }
}

void SkyDemo::handle(const sf::Event &event) {
  if (event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::Key::F)
      sky.spawnPlane(0, {200, 200}, 0, sky::PlaneTuning());
  }

  ctrlState.handle(event);
  if (plane->state) ctrlState.setState(*plane->state);
}

