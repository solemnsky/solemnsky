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
#include "client/client.hpp"
#include "style.hpp"
#include "elements.hpp"

/**
 * ClientUiState.
 */

ClientUiState::ClientUiState() :
    focusedPage(PageType::Home),
    pageFocusing(false),
    gameFocusing(false),
    pageFocusFactor(0),
    gameFocusFactor(0) {}

bool ClientUiState::pageFocused() const {
  return pageFocusFactor == 1 and gameFocusFactor == 0;
}

bool ClientUiState::gameFocused() const {
  return gameFocusFactor == 1;
}

bool ClientUiState::menuFocused() const {
  return pageFocusFactor == 0;
}

/**
 * ClientShared.
 */

ClientShared::ClientShared(Client &client, const ui::AppRefs &references) :
    client(client), references(references) {}

template<typename T>
std::vector<std::pair<T, bool>>
bindingsFromEvent(const sf::Event &event,
                 const std::map<InputAction, T> &map) {
  std::vector<InputAction> actions = InputAction::actionsForEvent(event);
  std::vector<std::pair<T, bool>> makes;

  for (const InputAction &action : actions) {
    bool make = action.isMake(event);
    const auto &find = map.find(action);

    if (find != map.end()) {
      makes.push_back(std::make_pair(find->second, make));
}
  }

  return makes;
}

std::vector<std::pair<sky::Action, bool>>
ClientShared::findSkyActions(const sf::Event &event) const {
  return bindingsFromEvent(event, client.settings.bindings.skyBindings);
}
std::vector<std::pair<ui::ClientAction, bool>>
ClientShared::findClientActions(const sf::Event &event) const {
  return bindingsFromEvent(event, client.settings.bindings.clientBindings);
}

const Settings &ClientShared::getSettings() const {
  return client.settings;
}

const Game *ClientShared::getGame() const {
  return client.game.get();
}

const ClientUiState &ClientShared::getUi() const {
  return client.uiState;
}

void ClientShared::beginGame(std::unique_ptr<Game> &&game) {
  client.beginGame(std::move(game));
}

void ClientShared::blurGame() {
  client.blurGame();
}

void ClientShared::focusGame() {
  client.focusGame();
}

void ClientShared::exitGame() {
  client.exitGame();
}

void ClientShared::focusPage(const PageType type) {
  client.focusPage(type);
}

void ClientShared::blurPage() {
  client.blurPage();
}

void ClientShared::changeSettings(const ui::SettingsDelta &settings) {
  client.changeSettings(settings);
}


