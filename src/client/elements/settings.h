/**
 * Managing settings, changes in settings, reading and writing settings to
 * disk. Settings are anything that changes how the client works and should
 * be expected to be saved to disk.
 */
#ifndef SOLEMNSKY_SETTINGS_H
#define SOLEMNSKY_SETTINGS_H

#include "util/types.h"

/**
 * Client settings, for everything from graphics preferences to player name.
 * Saved and loaded from disk.
 */
struct Settings {
  Settings(); // initialize with meaningful default settings

  bool readFromFile(std::string filepath);
  bool writeToFile(std::string filepath);

  std::string nickname;
};

/**
 * A delta between a past and a current Settings instantiation, so we know
 * exactly what we have to change.
 */
struct SettingsDelta {
  /**
   * Construct it as the delta between two object, or populate the optional
   * fields yourself.
   */
  SettingsDelta(const Settings &oldSettings, const Settings &newSettings);
  SettingsDelta();

  /**
   * Data.
   */
  optional<std::string> nickname;
};


#endif //SOLEMNSKY_SETTINGS_H
