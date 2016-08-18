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
#include "clientshared.hpp"

/**
 * There are various elements in our client that need to have access to the
 * ClientShared object and get a notification when settings change.
 */
class ClientComponent {
 protected:
  ClientShared &shared;
  std::vector<ClientComponent *> childComponents;

  void areChildComponents(
      std::initializer_list<ClientComponent *> newChildren);

 public:
  ClientComponent(ClientShared &shared);

  // Interface API.
  virtual void onChangeSettings(const ui::SettingsDelta &settings);

};

/**
 * Pages are the portals through which users interface with the tutorial
 * periphery: looking for servers, editing settings, loading tutorial records,
 * etc.
 *
 * We have three pages in our menu interface. They are all persistent, in
 * that they always exist, with their state and tick() operations, whether they
 * are displayed or not.
 */
class Page: public ClientComponent, public ui::Control {
 public:
  Page(ClientShared &shared);

  // Interface API.
  virtual void onFocus() { }
  virtual void onBlur() { }
  void drawBackground(ui::Frame &f);

};

/**
 * A game, when it exists, occupies a singular position in the client. There
 * are various game-like interfaces we can provide: a tutorial, a multiplayer
 * client, etc.
 */
class Game: public ClientComponent, public ui::Control {
 public:
  Game(ClientShared &shared, const std::string &name);
  virtual ~Game() { }

  // Interface API.
  virtual void onBlur() { }
  virtual void onFocus() { }
  // try to exit, set Control::quitting flag eventually
  // (a multiplayer connection may want to do this at its leisure to
  // disconnect gracefully)
  virtual void doExit() = 0;
  virtual void printDebugLeft(Printer &p);
  virtual void printDebugRight(Printer &p);
  virtual std::string getQuittingReason() const; // describe briefly why a quit occured

  std::string name; // identifier of the type of game being played
  std::string status; // brief status of the game

};
