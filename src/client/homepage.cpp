#include "homepage.h"
#include "base/resources.h"

void HomePage::tick(float delta) {

}

void HomePage::render(ui::Frame &f) {
  f.drawSprite(textureOf(Res::Title), {0, 0}, {0, 0, 1600, 900});
}

void HomePage::handle(const sf::Event &event) {

}
