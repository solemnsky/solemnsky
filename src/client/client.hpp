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
 * The client top-level; the meaningful application.
 */
#pragma once
#include "client/sandbox/sandbox.hpp"
#include "multiplayer/multiplayer.hpp"
#include "homepage.hpp"
#include "settingspage.hpp"
#include "listingpage.hpp"

/**
 * The main client app.
 * Its various components (the various pages and potential running game) are
 * neatly modularized; see client/elements/elements.h. This is simply the glue
 * that brings everything together.
 */
class Client: public ui::Control {
  friend struct ClientShared;
 private:
  // Buttons.
  ui::Button backButton, // for exiting menus, lower right
      closeButton, // for closing the current sandbox, lower left
      quitButton, // quitting solemnsky
      aboutButton; // for seeing the about screen

  // State.
  std::unique_ptr<class Game> game;
  ClientUiState uiState;
  ClientShared shared; // interface reference

  // Pages.
  ui::Transformed<HomePage> homePage;
  ui::Transformed<ListingPage> listingPage;
  ui::Transformed<SettingsPage> settingsPage;
  bool tryingToQuit; // trying to exit, waiting on the game to close

  // Reason that we quitted the game.
  std::string quittingReason;
  Scheduler quitDisplayTimer;

  // Performance profiling.
  Scheduler profileScheduler;
  ui::ProfilerSnapshot profilerSnap;

  // Internal helpers.
  void forAllPages(std::function<void(Page &)> f);
  ui::TransformedBase &referencePageBase(const PageType type);
  Page &referencePage(const PageType type);

  // Render subroutines.
  void renderPage(ui::Frame &f, const PageType type,
                  const sf::Vector2f &offset, const std::string &name,
                  ui::TransformedBase &page);
  void renderUI(ui::Frame &f);
  void renderGame(ui::Frame &f);

 public:
  Client(const ui::AppRefs &references);

  // Control impl.
  void poll() override final;
  void tick(TimeDiff delta) override final;
  void render(ui::Frame &f) override final;
  bool handle(const sf::Event &event) override final;
  void reset() override final;
  void signalRead() override final;
  void signalClear() override final;

  // UI Methods, callable from children UI objects.
  void beginGame(std::unique_ptr<Game> &&game);
  void focusGame();
  void blurGame();
  void exitGame();

  void focusPage(const PageType page);
  void blurPage();

  // Changing settings, propogating the delta everywhere.
  void changeSettings(const ui::SettingsDelta &settingsDelta);

};

