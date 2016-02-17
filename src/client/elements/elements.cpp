#include "elements.h"

/**
 * Page.
 */

Page::Page(ClientShared &state) : shared(state) { }

void Page::drawBackground(ui::Frame &f) {

  f.withTransform(
      sf::Transform().scale(style.xFactor, style.yFactor, 800, 450),
      [&]() {
        f.drawSprite(textureOf(Res::Title), {0, 0}, {0, 0, 1600, 900});
      });
}

/**
 * Game.
 */

Game::Game(ClientShared &shared,
           const std::string &name) :
    shared(shared),
    name(name) { }

