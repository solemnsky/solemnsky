#include "skydemo.h"
#include "demo.h"
#include "base/resources.h"


sf::Texture getBackground() {
  sf::Texture texture;
//  texture.loadFromFile(getIdPath("render-2d/placeholder/title.png"));
  texture.loadFromFile(getIdPath("render-3d/test_1/player_200.png"));
}

SkyDemo::SkyDemo() : background(getBackground()) { }

void SkyDemo::tick(float delta) {
  button.tick(delta);
}

void SkyDemo::render(ui::Frame &f) {
  f.drawSprite(background, sf::Vector2f(0, 0), sf::IntRect(0, 0, 1600, 900));
  button.render(f);
}

void SkyDemo::handle(sf::Event event) {
  button.handle(event);
  if (button.isClicked)
    next = std::make_shared<Demo>();
}

