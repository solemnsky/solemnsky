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
 * Configuration values for the launcher and application.
 */
#pragma once
#include "engine/types.hpp"
#include "inputaction.hpp"

namespace ui {

/**
 * Actions a key-press can achieve in a client. Like sky::Action but for the
 * client instead of the sky.
 */
enum class ClientAction {
  Spawn, Chat, Scoreboard, MAX
};

void forClientActions(std::function<void(const ClientAction)> fn);

std::string showClientAction(const ClientAction action);

/**
 * Key bindings for the client.
 */
struct ActionBindings {
  ActionBindings();

  optional<InputAction> lookupBinding(sky::Action) const;
  optional<InputAction> lookupClientBinding(ClientAction) const;

  std::map<InputAction, sky::Action> skyBindings;
  std::map<InputAction, ClientAction> clientBindings;

  // Access key bindings.
  std::vector<std::pair<sky::Action, bool>> findSkyActions(
      const sf::Event &event) const;
  std::vector<std::pair<ui::ClientAction, bool>> findClientActions(
      const sf::Event &event) const;

};

/**
 * Client settings, for everything from graphics preferences to player name.
 * Saved to and loaded from disk.
 */
struct Settings {
  Settings(const std::string &filepath);
  Settings() = delete;

  void writeToFile(const std::string &filepath) const;

  // Launcher settings.
  bool fullscreen;
  sf::Vector2u resolution;

  // Client settings.
  bool enableDebug;
  std::string nickname;
  ActionBindings bindings;

};

/**
 * A delta in a Settings.
 */
struct SettingsDelta {
  // delta between two objects
  SettingsDelta(const Settings &oldSettings, const Settings &newSettings);
  // null delta
  SettingsDelta();

  void apply(Settings &settings) const;

  optional<bool> fullscreen;
  optional<bool> enableDebug;
  optional<std::string> nickname;
  optional<ActionBindings> bindings;
};

}
