#include "elements.h"
#include "style.h"

/**
 * Page.
 */

Page::Page(ClientShared &shared) : shared(shared) { }

void Page::drawBackground(ui::Frame &f) {
  const float margins = style.base.pageMargins;
  f.drawRect({margins, margins},
             {1600 - margins, 900 - margins}, style.base.pageBgColor);
}

/**
 * Game.
 */

Game::Game(ClientShared &shared,
           const std::string &name) :
    shared(shared),
    name(name) { }
