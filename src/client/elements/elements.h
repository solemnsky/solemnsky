/**
 * Clearly divided classes of GUI elements that make up the client.
 */
#pragma once
#include "clientshared.h"

/**
 * Pages are the portals through which users interface with the tutorial
 * periphery: looking for servers, editing settings, loading tutorial records,
 * etc.
 *
 * We have three pages in our menu interface. They are all persistent, in
 * that they always exist, with their state and tick() operations, whether they
 * are displayed or not.
 */
class Page: public ui::Control {
 protected:
  ClientShared &shared;

 public:
  Page(ClientShared &shared);
  // a page's lifetime is never handled by a reference to a Page
  // so we need no virtual dtor

  virtual void onChangeSettings(const SettingsDelta &settings) = 0;
  virtual void onBlur() = 0;

  void drawBackground(ui::Frame &f);
};

/**
 * A game, when it exists, occupies a singular position in the client. There
 * are various game-like interfaces we can provide: a tutorial, a multiplayer
 * client, etc.
 */
class Game: public ui::Control {
 protected:
  ClientShared &shared;

 public:
  Game(ClientShared &shared, const std::string &name);
  virtual ~Game() { }

  virtual void onChangeSettings(const SettingsDelta &settings) = 0;
  virtual void onBlur() = 0;
  virtual void onFocus() = 0;
  // try to exit, set Control::quitting flag eventually
  // (a multiplayer connection may want to do this at its leisure to
  // disconnect gracefully)
  virtual void doExit() = 0;

  std::string name; // identifier of the type of game being played
  std::string status; // brief status of the game
};
