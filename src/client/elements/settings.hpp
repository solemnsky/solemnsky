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
 * Configuration values, persistent across reboots.
 */
#pragma once
#include <SFML/Window.hpp>
#include "sky/participation.h"
#include "util/types.h"

/**
 * Actions a key-press can achieve in a client. Like sky::Action but for the
 * client instead of the sky.
 */
enum class ClientAction {
  Spawn, Chat, Scoreboard, MAX
};

std::string showClientAction(const ClientAction action);

/**
 * Key bindings for the client.
 */
struct KeyBindings {
  KeyBindings();

  optional<sf::Keyboard::Key> lookupBinding(sky::Action) const;
  optional<sf::Keyboard::Key> lookupClientBinding(ClientAction) const;

  std::map<sf::Keyboard::Key, sky::Action> skyBindings;
  std::map<sf::Keyboard::Key, ClientAction> clientBindings;
};

/**
 * Client settings, for everything from graphics preferences to player name.
 * Saved to and loaded from disk.
 */
struct Settings {
  Settings(); // initialize with meaningful default settings

  void readFromFile(const std::string &filepath);
  void writeToFile(const std::string &filepath);

  bool enableDebug;
  std::string nickname;
  KeyBindings bindings;
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

  optional<bool> enableDebug;
  optional<std::string> nickname;
  optional<KeyBindings> bindings;
};
