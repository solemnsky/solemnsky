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
 * Component of the multiplayer client corresponding to ArenaMode::Lobby.
 */
#pragma once
#include "multiplayercore.hpp"
#include "ui/widgets.hpp"

/**
 * In the lobby, we can talk to people, see teams, change teams, and vote for
 * the game to start.
 */

class MultiplayerLobby: public MultiplayerView {
 private:
  ui::Button specButton;
  ui::Button redButton;
  ui::Button blueButton;

 public:
  MultiplayerLobby(ClientShared &shared, MultiplayerCore &connection);

  // Control impl.
  void tick(float delta) override final;
  void render(ui::Frame &f) override final;
  bool handle(const sf::Event &event) override final;
  void reset() override final;
  void signalRead() override final;
  void signalClear() override final;

  // ClientComponent impl.
  void onChangeSettings(const ui::SettingsDelta &settings) override final;

  // MultiplayerView impl.
  void handleClientAction(const ui::ClientAction action, const bool state) override final;

};
