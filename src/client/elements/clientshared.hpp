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
 * Interface that children have to the top-level Client.
 */
#pragma once
#include "engine/sky/participation.hpp"
#include "ui/control.hpp"
#include "settings.hpp"
#include "elements.hpp"

class Client;

enum class PageType {
  Home, Listing, Settings
};

/**
 * The interesting UI state of the client.
 */
struct ClientUiState {
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
 * This is the interface that all ClientComponents have to interact with the top-level
 * Client state. It also propogates an ui::AppRefs reference -- ClientShared substitutes AppRefs
 * for ClientComponents.
 */
struct ClientShared {
  friend class Client;
 private:
  Client &client;

  // Constructed by client.
  ClientShared(Client &client, const ui::AppRefs &references);

 public:
  const ui::AppRefs &references;

  // Query client data.
  const Settings &getSettings() const;
  const Game *getGame() const;
  const ClientUiState &getUiState() const;

  // Call client methods.
  void beginGame(std::unique_ptr<Game> &&game);
  void blurGame();
  void focusGame();
  void exitGame();

  void focusPage(const PageType type);
  void blurPage();

  void changeSettings(const SettingsDelta &settings);

  // Access key bindings.
  optional<std::pair<sky::Action, bool>> triggerSkyAction(
      const sf::Event &event) const;
  optional<std::pair<ClientAction, bool>> triggerClientAction(
      const sf::Event &event) const;


};

