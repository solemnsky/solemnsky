//
// Created by Chris on 1/19/2016.
//

#include "settingspage.h"
#include "clientstate.h"

/****
 * Settings.
 */

Settings::Settings() :
    debugInfo(true),
    fullscreen(false) { }// the defaults

Settings::Settings(std::string filepath) {
  readFromFile(filepath);
}

void Settings::readFromFile(std::string filepath) {
  debugInfo = true;
}

void Settings::writeToFile(std::string filepath) {
  // write to file
}

/****
 * ClientState
 */

ClientState::ClientState() :
    focusedPage(PageType::Home),
    pageFocused(false) { }

void ClientState::focusGame() {
  pageFocused = false;
  game->inFocus = true;
}
