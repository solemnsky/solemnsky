#include "settings.h"

/**
 * Settings.
 */

Settings::Settings() :
    nickname("nameless plane"),
    enableDebug(false) { }

bool Settings::readFromFile(std::string filepath) {
  return true;
}

bool Settings::writeToFile(std::string filepath) {
  // write to file
  return true;
}

/**
 * SettingsDelta.
 */

SettingsDelta::SettingsDelta() { }

SettingsDelta::SettingsDelta(const Settings &oldSettings,
                             const Settings &newSettings) {
  if (oldSettings.nickname != newSettings.nickname)
    nickname = newSettings.nickname;
  if (oldSettings.enableDebug != newSettings.enableDebug)
    enableDebug = newSettings.enableDebug;
}

void SettingsDelta::apply(Settings &settings) const {
  if (nickname) settings.nickname = *nickname;
  if (enableDebug) settings.enableDebug = *enableDebug;
}
