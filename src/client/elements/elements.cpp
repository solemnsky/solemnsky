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
 * ClientComponent.
 */

void ClientComponent::areChildComponents(
    std::initializer_list<ClientComponent *> newChildren) {
  for (const auto child : newChildren) childComponents.push_back(child);
}

ClientComponent::ClientComponent(ClientShared &shared) :
    shared(shared) { }

void ClientComponent::onChangeSettings(const SettingsDelta &settings) {
  for (const auto child : childComponents)
    child->onChangeSettings(settings);
}

/**
 * Page.
 */

Page::Page(ClientShared &shared) :
    ClientComponent(shared),
    ui::Control(shared.references) {}

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
    ClientComponent(shared),
    ui::Control(shared.references),
    name(name) { }

void Game::printDebugLeft(Printer &p) {
  p.printLn("undefined!");
}

void Game::printDebugRight(Printer &p) {
  p.printLn("undefined!");
}
