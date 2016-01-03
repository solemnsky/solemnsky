#include "skydemo.h"
#include "demo.h"
#include "base/resources.h"
#include "ui/sheet.h"

sf::Texture getBackground() {
  sf::Texture texture;
  texture.loadFromFile(filepathTo(Res::TitleScreen));
  return texture;
}

SkyDemo::SkyDemo() : background(getBackground()) { }

void SkyDemo::tick(float delta) {
  if (animTicker.tick(delta)) animVal = (animVal + 1) % sheet.count;
  sky.tick(delta);
}

void SkyDemo::render(ui::Frame &f) {
  f.drawSprite(background, sf::Vector2f(0, 0), sf::IntRect(0, 0, 1600, 900));
  sheet.drawIndex(f, sf::Vector2f(800, 450), sf::Vector2f(200, 200), animVal);

//  sky.render(sf::Vector2f(0, 0));
}

void SkyDemo::handle(sf::Event event) {
}

