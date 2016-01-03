#include "skydemo.h"
#include "demo.h"
#include "base/resources.h"

sf::Texture getBackground() {
  sf::Texture texture;
  texture.loadFromFile(getIdPath("render-2d/placeholder/title.png"));
  return texture;
}

SkyDemo::SkyDemo() : background(getBackground()) { }

void SkyDemo::tick(float delta) {
}

void SkyDemo::render(ui::Frame &f) {
  f.drawSprite(background, sf::Vector2f(0, 0), sf::IntRect(0, 0, 1600, 900));
}

void SkyDemo::handle(sf::Event event) {
}

