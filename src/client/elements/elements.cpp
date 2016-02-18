#include "elements.h"

/**
 * Page.
 */

Page::Page(ClientShared &state) : shared(state) { }

void Page::drawBackground(ui::Frame &f) {
  f.drawRect({style.margins, style.margins},
             {1600 - style.margins, 900 - style.margins}, style.bgColor);
}

/**
 * Game.
 */

Game::Game(ClientShared &shared,
           const std::string &name) :
    shared(shared),
    name(name) { }

