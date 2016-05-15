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
 * A tutorial to introduce new players to the mechanics.
 */
#pragma once
#include "client/elements/elements.h"
#include "sky/sky.h"
#include "client/skyrender.h"

class Tutorial : public Game {
private:
  sky::Arena arena;
  sky::SkyHandle skyManager;
  sky::SkyRender skyRender;

  sky::Player *player;
  const sky::Participation *participation;

public:
  Tutorial(ClientShared &state);

  /**
   * Game interface.
   */
  void onChangeSettings(const SettingsDelta &settings) override;
  void onBlur() override;
  void onFocus() override;
  void doExit() override;

  /**
   * Control interface.
   */
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
  void reset() override;
  void signalRead() override;
  void signalClear() override;
};
