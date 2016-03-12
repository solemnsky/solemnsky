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
  const struct Style {
    float unfocusedPageScale = 500.0f / 1600.0f;

    sf::Vector2f
        homeOffset{182.812, 121.875},
        settingsOffset{917.187, 121.875},
        listingOffset{550.000, 495.250},
        quitButtonOffset{182.812, 610.875},
        aboutButtonOffset{1217.188, 610.875},
        closeButtonOffset{1300, 0},
        backButtonOffset{1067.18, 850};
    // see the schema in media/source2d/board.png

    sf::Color pageUnderlayColor{0, 0, 0, 20};

    int descriptionFontSize = 50; // bits of text that describe things
    int descriptionNegativeMargin = 10;

    ui::Button::Style highButtonStyle() const;
    ui::Button::Style lowButtonStyle() const;

    std::string backButtonText = "main menu",
        quitButtonText = "quit",
        closeButtonText = "close tutorial",
        aboutButtonText = "about";

    float menuInGameFade = 0.7; // the opacity of the UI when we open it
    // by escaping from the tutorial (this opacity factor dissapates when a page
    // is focused)

    Style() { }
  } style;

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
   * Pages.
   */
  HomePage homePage;
  ListingPage listingPage;
  SettingsPage settingsPage;

  std::vector<std::pair<sf::FloatRect, PageType>> pageRects;

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
