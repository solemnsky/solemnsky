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
 * The client top-level; connects the user to the game.
 */
#pragma once
#include "tutorial/tutorial.h"
#include "multiplayer/multiplayer.h"
#include "homepage.h"
#include "settingspage.h"
#include "listingpage.h"

/**
 * The main client app.
 * Its various components (the various pages and potential running game) are
 * neatly modularized; see client/elements/elements.h. This is simply the glue
 * that brings everything together.
 */
class Client: public ui::Control {
 private:
  // Buttons.
  ui::Button backButton, // for exiting menus, lower right
      closeButton, // for closing the current tutorial, lower left
      quitButton, // quitting solemnsky
      aboutButton; // for seeing the about screen

  // Shared state.
  ClientShared shared;

  // Misc ui.
  HomePage homePage;
  ListingPage listingPage;
  SettingsPage settingsPage;
  bool tryingToQuit; // trying to exit, waiting on the game to close

  Cooldown profilerCooldown;
  ui::ProfilerSnapshot profilerSnap;

  // Internal helpers.
  void forAllPages(std::function<void(Page &)> f);
  Page &referencePage(const PageType type);
  void drawPage(ui::Frame &f, const PageType type,
                const sf::Vector2f &offset, const std::string &name,
                ui::Control &page);
  void drawUI(ui::Frame &f);
  void drawGame(ui::Frame &f);

 public:
  Client(const ui::AppRefs &references);

  // Control impl.
  bool poll() override;
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
  void reset() override;
  void signalRead() override;
  void signalClear() override;

  // UI API.
  void beginGame(std::unique_ptr<Game> &&game);
  void focusGame();
  void blurGame();
  void exitGame();

  void focusPage(const PageType type);
  void blurPage();

  void changeSettings(const SettingsDelta &settings);
};
