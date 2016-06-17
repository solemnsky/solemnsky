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
 * Sandbox for testing the engine and tuning parameters.
 */
#pragma once
#include "client/elements/elements.hpp"
#include "sky/sky.hpp"
#include "client/skyrender.hpp"
#include "sky/debugview.hpp"
#include "ui/widgets.hpp"


/**
 * A command that can be executed in the sandbox.
 */
struct SandboxCommand {
  SandboxCommand() = default;
};

/**
 * The sandbox -- a sort of boring Game.
 */
class Sandbox : public Game {
private:
  // Engine state.
  sky::Arena arena;
  sky::SkyHandle skyHandle;
  sky::DebugView debugView;
  optional<sky::SkyRender> skyRender;

  sky::Player *player;

  // UI features.
  ui::TextEntry commandEntry;

  // Submethods.
  void startGame();
  void stopGame();

public:
  Sandbox(ClientShared &state);

  // Game impl.
  void onChangeSettings(const SettingsDelta &settings) override;
  void onBlur() override;
  void onFocus() override;
  void doExit() override;
  virtual void printDebugLeft(Printer &p) override;
  virtual void printDebugRight(Printer &p) override;

  // Control impl.
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
  void reset() override;
  void signalRead() override;
  void signalClear() override;
};
