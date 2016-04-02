/**
 * Configuration values, persistent across reboots.
 */
#pragma once
#include <SFML/Window.hpp>
#include "sky/plane.h"
#include "util/types.h"

/**
 * Actions a key-press can achieve in a client.
 */
enum class ClientAction {
  Chat, Scoreboard
};

/**
 * Key bindings for the client.
 */
struct KeyBindings {
  KeyBindings();

  std::map<sf::Keyboard::Key, sky::Action> skyBindings;
  std::map<sf::Keyboard::Key, ClientAction> clientBindings;
};

bool operator==(const KeyBindings &x, const KeyBindings &y);

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
