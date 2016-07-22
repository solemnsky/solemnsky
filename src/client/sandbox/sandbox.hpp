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
#include "engine/sky/sky.hpp"
#include "client/skyrender.hpp"
#include "engine/debugview.hpp"
#include "ui/widgets.hpp"
#include "client/elements/clientui.hpp"
#include "engine/event.hpp"

/**
 * A command that can be executed in the sandbox.
 */
struct SandboxCommand {
 public:
  enum Type {
    Start, // start a game
    Stop, // stop the game
    Tune, // modify or query some PlaneTuning value
    DumpTuning // dump the tuning data to disk
  };

 private:
  SandboxCommand(const Type type);

 public:
  SandboxCommand() = delete;

  Type type;
  optional<std::string> mapName; // on Start command
  optional<std::string> tuningParam;
  optional<float> tuningValue;

  static optional<SandboxCommand> parseCommand(const std::string &input);

};

class Sandbox;

/**
 * ArenaLogger used by the sandbox to catch events.
 */
class SandboxLogger: public sky::ArenaLogger {
 private:
  Sandbox &sandbox;

 protected:
  virtual void onEvent(const sky::ArenaEvent &event) override;

 public:
  SandboxLogger(sky::Arena &arena, Sandbox &sandbox);

};

/**
 * The sandbox -- a sort of boring Game.
 */
class Sandbox: public Game {
  friend class SandboxLogger;
 private:
  // Engine state.
  sky::Arena arena;
  sky::SkyHandle skyHandle;
  sky::DebugView debugView;
  optional<sky::SkyRender> skyRender;
  sky::PlaneTuning spawnTuning; // tuning to use on plane spawn
  sky::Player *player;

  SandboxLogger logger;

  // UI features.
  MessageInteraction messageInteraction;

  // Printers.
  EnginePrinter enginePrinter;
  ClientPrinter clientPrinter;
  GameConsolePrinter consolePrinter;

  // Submethods.
  void startHandle();
  void stopHandle();

  // Commands
  void runCommand(const SandboxCommand &command);

  // Render submethods.
  void displayStatus(ui::Frame &f, const std::string &status);

 public:
  Sandbox(ClientShared &state);

  // Game impl.
  void onChangeSettings(const SettingsDelta &settings) override final;
  void onBlur() override final;
  void onFocus() override final;
  void doExit() override final;
  virtual void printDebugLeft(Printer &p) override final;
  virtual void printDebugRight(Printer &p) override final;

  // Control impl.
  void tick(const TimeDiff delta) override final;
  void render(ui::Frame &f) override final;
  bool handle(const sf::Event &event) override final;
  void reset() override final;
  void signalRead() override final;
  void signalClear() override final;
};
