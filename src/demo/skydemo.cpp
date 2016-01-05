#include "skydemo.h"
#include "demo.h"
#include "base/resources.h"
#include "ui/sheet.h"

SkyDemo::SkyDemo() :
    renderMan(&sky),
    animTicker{0.05f},
    animVal{0} {
  sky.joinPlane(0, sky::PlaneTuning());
}

void SkyDemo::tick(float delta) {
  renderMan.tick(delta);
  sky.tick(delta);
}

void SkyDemo::render(ui::Frame &f) {
  f.drawSprite(textureFrom(Res::TitleScreen),
               sf::Vector2f(0, 0), sf::IntRect(0, 0, 1600, 900));
  renderMan.render(f, sf::Vector2f(0, 0));
}

void SkyDemo::handle(sf::Event event) {
  if (event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::Key::F);
//      sky.getPlane(0)->spawn(sf::Vector2f(200, 200), 0, sky::PlaneTuning());
  }
}

