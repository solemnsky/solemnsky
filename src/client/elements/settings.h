/**
 * Configuration values, persistent across reboots.
 */
#pragma once
#include <SFML/Window.hpp>
#include "sky/plane.h"
#include "util/types.h"

/**
 * An action accomplished by a key our client.
 */
struct KeyAction {
  enum class Type {
    Chat, Scoreboard, SkyAction
  } type;

 private:
  KeyAction(const Type type);

 public:
  optional<sky::Action> skyAction;

  static KeyAction Chat();
  static KeyAction Scoreboard();
  static KeyAction SkyAction(const sky::Action action);
};

/**
 * Key bindings for the client.
 */
struct KeyBindings {
  KeyBindings();

  // use a bimap for this?
  std::map<sf::Keyboard::Key, KeyAction>;
};

/**
 * Client settings, for everything from graphics preferences to player name.
 * Saved and loaded from disk.
 */
struct Settings {
  Settings(); // initialize with meaningful default settings

  void readFromFile(std::string filepath);
  void writeToFile(std::string filepath);

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
