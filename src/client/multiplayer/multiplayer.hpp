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
 * Game played on a remote server.
 */
#pragma once
#include "multiplayercore.hpp"

/**
 * Top-level multiplayer client.
 */
class Multiplayer: public Game, public ConnectionListener {
 private:
  // Arena connection state / protocol implementation and the current
  // MultiplayerView, presenting the user with a direct interface.
  MultiplayerCore core;
  std::unique_ptr<MultiplayerView> view;
  void loadView(const sky::ArenaMode arenaMode);

  // Font.
  const sf::Font &hudFont;

 public:
  Multiplayer(ClientShared &shared,
              const std::string &serverHostname,
              const unsigned short serverPort);

  // ConnectionListener impl.
  void onConnect() override final;
  void onLoadMode(const sky::ArenaMode mode) override final;
  void onStartGame() override final;
  void onEndGame() override final;

  // Game impl.
  void onChangeSettings(const SettingsDelta &settings) override final;
  void onBlur() override final;
  void onFocus() override final;
  void doExit() override final;
  void printDebugLeft(Printer &p) override final;
  void printDebugRight(Printer &p) override final;

  // Control impl.
  bool poll() override;
  void tick(const TimeDiff delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
  void reset() override;
  void signalRead() override;
  void signalClear() override;

};
