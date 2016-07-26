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
 * Splash screen and multiplayer game interface to display when the arena is in game mode.
 */
#pragma once
#include "multiplayercore.hpp"
#include "ui/widgets.hpp"

/**
 * The game interface itself. Assumes that the Sky is instantiated.
 * This is the most important thing in the client.
 */
class MultiplayerGame : public MultiplayerView {
 private:
  bool scoreboardFocused;
  sky::SkyRender skyRender;
  const sky::Participation &participation;

  // Helper subroutines.
  void doClientAction(const ui::ClientAction action, const bool state);
  void printScores(ui::TextFrame &tf, const sky::Team team);
  void printSpectators(ui::TextFrame &tf);
  void renderScoreboard(ui::Frame &f);

 public:
  MultiplayerGame(ClientShared &shared, MultiplayerCore &core);

  // Control impl.
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
  void signalRead() override;
  void signalClear() override;

};

/**
 * MultiplayerGameHandle waits for this to quit before moving on to the game.
 */
class MultiplayerSplash : public MultiplayerView {
 private:
 public:
  MultiplayerSplash(ClientShared &shared, MultiplayerCore &core);

  // Control impl.
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
  void signalRead() override;
  void signalClear() override;

};
