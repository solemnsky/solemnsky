/**
 * Client state datatypes / saving some settings to disk.
 */
#ifndef SOLEMNSKY_CLIENTSTATE_H
#define SOLEMNSKY_CLIENTSTATE_H

#include "ui/ui.h"
#include "game/game.h"

/**
 * The persistent settings that tweaks how a client works on conceivably
 * every level. A field goes in this struct <=> that field should be saved to
 * disk and reloaded on client restart.
 */
struct Settings {
  Settings();
  Settings(std::string filepath);

  void readFromFile(std::string filepath);
  void writeToFile(std::string filepath);

  bool fullscreen;
  bool debugInfo;
};

/****
 * ClientState: Global state for one instance of the client, accessible just
 * about * everywhere.
 */

enum class PageType {
  Home, Listing, Settings
};

struct ClientState {
  ClientState();

  /**
   * ui::AppState and Settings.
   */
  ui::AppState *appState;
  Settings settings;
  float uptime;

  /**
   * The running game.
   */
  std::unique_ptr<Game> game;

  inline bool gameInFocus() {
    if (game) return game->inFocus;
    return false;
  }

  /**
   * Menu UI state, helpers.
   */
  PageType focusedPage;
  bool pageFocused;

  void focusGame();
};


#endif //SOLEMNSKY_CLIENTSTATE_H
