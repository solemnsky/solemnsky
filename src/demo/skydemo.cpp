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
}

SkyDemo::SkyDemo() :
    renderMan(&sky),
    animTicker{0.05f},
    animVal{0} {
  plane = sky.joinPlane(0, sky::PlaneTuning());
}

void SkyDemo::tick(float delta) {
  renderMan.tick(delta);
  sky.tick(delta);
}

void SkyDemo::render(ui::Frame &f) {
  f.drawSprite(textureOf(Res::TitleScreen),
               sf::Vector2f(0, 0), sf::IntRect(0, 0, 1600, 900));
  renderMan.render(f, sf::Vector2f(0, 0));
}

void SkyDemo::handle(const sf::Event &event) {
  if (event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::Key::F)
      plane->spawn({200, 200}, 0, sky::PlaneTuning());
  }

  ctrlState.handle(event);
  if (plane->state) ctrlState.setState(*plane->state);
}

