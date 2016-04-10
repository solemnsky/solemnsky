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
 * Its various components (the various pages and potential running tutorial) are
 * neatly modularized; see client/elements/elements.h. This is simply the glue
 * that brings everything together.
 */
class Client: public ui::Control {
 private:
  // buttons
  ui::Button backButton, // for exiting menus, lower right
      closeButton, // for closing the current tutorial, lower left
      quitButton, // quitting solemnsky
      aboutButton; // for seeing the about screen

  // shared state
  ClientShared shared;

  // misc ui
  HomePage homePage;
  ListingPage listingPage;
  SettingsPage settingsPage;
  bool tryingToQuit; // trying to exit, waiting on the game to close

  Cooldown profilerCooldown;
  ui::ProfilerSnapshot profilerSnap;

  // helpers
  void forAllPages(std::function<void(Page &)> f);
  Page &referencePage(const PageType type);
  void drawPage(ui::Frame &f, const PageType type,
                const sf::Vector2f &offset, const std::string &name,
                ui::Control &page);
  void drawUI(ui::Frame &f);
  void drawGame(ui::Frame &f);

 public:
  Client(ui::AppState &appState);

  /**
   * Control interface.
   */
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
  void reset() override;
  void signalRead() override;
  void signalClear() override;

  /**
   * UI methods.
   */
  void beginGame(std::unique_ptr<Game> &&game);
  void focusGame();
  void blurGame();
  void exitGame();

  void focusPage(const PageType type);
  void blurPage();

  void changeSettings(const SettingsDelta &settings);
};
