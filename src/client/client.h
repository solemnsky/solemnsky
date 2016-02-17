/**
 * Top-level client, designed to connect the end-user to the Sky efficiently.
 */
#ifndef SOLEMNSKY_CLIENT_H
#define SOLEMNSKY_CLIENT_H

#include "game/tutorial.h"
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
  const struct Style {
    float unfocusedPageScale = 500.0f / 1600.0f;

    sf::Vector2f
        homeOffset{182.812, 121.875},
        settingsOffset{917.187, 121.875},
        listingOffset{550.000, 495.250},
        quitButtonOffset{182.812, 610.875},
        aboutButtonOffset{1217.188, 610.875},
        backButtonOffset{1400, 850};
    // see the schema in media/source2d/board.png

    sf::Color pageUnderlayColor{0, 0, 0, 20};

    int descriptionFontSize = 50;
    int descriptionNegativeMargin = 10;

    ui::Button::Style backButtonStyle() const;
    ui::Button::Style quitButtonStyle() const;
    ui::Button::Style aboutButtonStyle() const;

    std::string backButtonText = "go back",
        quitGameText = "quit game",
        quitAppText = "quit solemnsky",
        aboutButtonText = "about";

    Style() { }
  } style;

  /**
   * Buttons.
   */
  ui::Button backButton; // for exiting menus, lower right
  ui::Button quitButton; // for exiting arena / quitting the game, top left
  ui::Button aboutButton; // for seeing the about screen

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
