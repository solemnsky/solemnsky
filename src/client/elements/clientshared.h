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
/**
 * The place where the whole client converges.
 */
#pragma once
#include "sky/participation.h"
#include "ui/control.h"
#include "settings.h"

class Client;

enum class PageType {
  Home, Listing, Settings
};

/**
 * Some UI state of our client; the various transitions and menu modes.
 */
class ClientUiState {
 private:
  friend class Client;

  void focusGame();
  void blurGame();
  void focusPage(PageType page);
  void blurPage();

  void tick(float delta);

 public:
  ClientUiState();

  PageType focusedPage;
  bool pageFocusing;
  bool gameFocusing;

  Clamped pageFocusFactor;
  Clamped gameFocusFactor;

  bool pageFocused() const;
  bool menuFocused() const;
  bool gameFocused() const;
};

/**
 * This is the shared core of the client, which all sub-elements refer to. It
 * has globally useful factors such as global settings, a unique_ptr to the Game
 * currently active, and UI methods that influence the whole Client.
 *
 * This is also used to propagate a ui::AppState.
 */
struct ClientShared {
  friend class Client;
 private:
  Client &client;

  // Constructed by client.
  ClientShared(Client &client, const ui::AppState &appState);

 public:
  const ui::AppState &appState;

  // State.
  Settings settings;
  std::unique_ptr<class Game> game;
  ClientUiState ui;

  // Access key bindings.
  optional<std::pair<sky::Action, bool>> triggerSkyAction(
      const sf::Event &event) const;
  optional<std::pair<ClientAction, bool>> triggerClientAction(
      const sf::Event &event) const;

  // Relays to top-level.
  void beginGame(std::unique_ptr<Game> &&game);
  void blurGame();
  void focusGame();
  void exitGame();

  void focusPage(const PageType type);
  void blurPage();

  void changeSettings(const SettingsDelta &settings);

};

