/**
 * solemnsky: the open-source multiplayer competitive 2D plane game
 * Copyright (C) 2016  Chris Gadzinski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "elements.h"
#include "style.h"

/**
 * Page.
 */

Page::Page(ClientShared &shared) :
    ui::Control(shared.appState),
    shared(shared) { }

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
    ui::Control(shared.appState),
    shared(shared),
    name(name) { }

