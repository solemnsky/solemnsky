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
 * Component of the multiplayer client corresponding to ArenaMode::Game.
 */
#pragma once
#include "multiplayercore.hpp"
#include "multiplayergame.hpp"
#include "client/skyrender.hpp"
#include "ui/widgets.hpp"

/**
 * Manager class, displaying the multiplayer game when resources and the sky are loaded, otherwise
 * displaying the multiplayer splash screen.
 */
class MultiplayerGameHandle : public MultiplayerView {
 private:
  MultiplayerSplash splash;
  optional<MultiplayerGame> gameView;

 public:
  MultiplayerGameHandle(ClientShared &shared, MultiplayerCore &core);

  // Control impl.
  virtual bool poll() override;
  virtual void tick(const TimeDiff delta) override;
  virtual void render(ui::Frame &f) override;
  virtual bool handle(const sf::Event &event) override;

};


