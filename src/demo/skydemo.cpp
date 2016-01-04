#include "skydemo.h"
#include "demo.h"
#include "base/resources.h"
#include "ui/sheet.h"

sf::Texture getBackground() {
  sf::Texture texture;
  texture.loadFromFile(filepathTo(Res::TitleScreen));
  return texture;
}

SkyDemo::SkyDemo() : background(getBackground()) {
  sky.joinPlane(0, sky::PlaneTuning());
}

void SkyDemo::tick(float delta) {
  if (animTicker.tick(delta)) animVal = (animVal + 1) % sheet.count;
  sky.tick(delta);
}

void SkyDemo::render(ui::Frame &f) {
  f.drawSprite(background, sf::Vector2f(0, 0), sf::IntRect(0, 0, 1600, 900));

//  if (sky::Plane *plane = sky.getPlane(0)) {
//    if (plane->state) {
//      sheet.drawIndex(f, plane->state->pos, sf::Vector2f(200, 200), animVal);
//    }
//    f.drawText(sf::Vector2f(800, 450), "You're dead, so sorry ...");
//  }
//  sky.render(sf::Vector2f(0, 0));
}

void SkyDemo::handle(sf::Event event) {
}

