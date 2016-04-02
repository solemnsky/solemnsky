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

/**
 * Client settings, for everything from graphics preferences to player name.
 * Saved and loaded from disk.
 */
struct Settings {
  Settings(); // initialize with meaningful default settings

  void readFromFile(const std::string &filepath);
  void writeToFile(const std::string &filepath);

  // general
  bool enableDebug;

  // player
  std::string nickname;

  // controls
  KeyBindings bindings;
};

/**
 * A delta between a past and a current Settings instantiation, so we can
 * reason about what we need to change.
 *
 * For example, when the player nickname is changed, the multiplayer client
 * should request a nickname change; if it isn't, this isn't necessary.
 */
struct SettingsDelta {
  /**
   * Construct it as the delta between two object, or populate the optional
   * fields yourself.
   */
  SettingsDelta(const Settings &oldSettings, const Settings &newSettings);
  SettingsDelta();

  void apply(Settings &settings) const;

  /**
   * Data.
   */
  optional<bool> enableDebug;
  optional<std::string> nickname;
};
