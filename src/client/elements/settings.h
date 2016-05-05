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
  Spawn, Chat, Scoreboard
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
