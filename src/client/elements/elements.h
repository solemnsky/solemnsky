/**
 * solemnsky: the open-source multiplayer competitive 2D plane game
 * Copyright (C) 2016  Chris Gadzinski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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

  // Internal API.
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
