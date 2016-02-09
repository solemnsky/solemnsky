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

  std::string preferredNickname;
};


#endif //SOLEMNSKY_SETTINGS_H
