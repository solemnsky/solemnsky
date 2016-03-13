/**
 * Top-level client, designed to connect the end-user to the Sky efficiently.
 */
#ifndef SOLEMNSKY_CLIENT_H
#define SOLEMNSKY_CLIENT_H

#include "tutorial/tutorial.h"
#include "client/multiplayer/multiplayer.h"
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
  /**
   * Buttons.
   */
  ui::Button backButton, // for exiting menus, lower right
      closeButton, // for closing the current tutorial, lower left
      quitButton, // quitting solemnsky
      aboutButton; // for seeing the about screen

  /**
   * Shared state.
   */
  ClientShared shared;

  /**
   * UI stuff.
   */
  HomePage homePage;
  ListingPage listingPage;
  SettingsPage settingsPage;

  std::vector<std::pair<sf::FloatRect, PageType>> pageRects;

  bool tryingToQuit; // trying to exit, waiting on the game to close

  /**
   * Helpers.
   */
  void forAllPages(std::function<void(Page &)> f);
  Page &referencePage(const PageType type);
  void drawPage(ui::Frame &f, const PageType type,
                const sf::Vector2f &offset, const std::string &name,
                ui::Control &page);

 public:
  Client();

  void attachState() override;

  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;

  void signalRead() override;
  void signalClear() override;

  /**
   * UI methods.
   */
  void resetUI(); // reset UI elements

  void beginGame(std::unique_ptr<Game> &&game);
  void exitGame();

  void focusGame();
  void blurGame();

  void focusPage(const PageType type);
  void blurPage();

  void changeSettings(const SettingsDelta &settings);
};

int main();

#endif //SOLEMNSKY_CLIENT_H
