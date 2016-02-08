/**
 * Top-level client, designed to connect the end-user to the Sky efficiently.
 */
#ifndef SOLEMNSKY_CLIENT_H
#define SOLEMNSKY_CLIENT_H

#include "game/tutorial.h"
#include "homepage.h"
#include "settingspage.h"
#include "listingpage.h"

class ClientUIState {
private:
  PageType focusedPage;
  bool pageFocusing;

  Clamped pageFocusAnim;
  Clamped gameFocusAnim;
public:
  ClientUIState();

  /**
   * Accessing.
   */
  float getPageFocusFactor(); // what's the focus factor of the page?
  float getGameFocusFactor(); // what's the factor of the game
  bool getPageFocused(); // is the page focused?
  bool getGameFocused(); // is the game focused?
  PageType getPageType(); // what page are we focused on?

  /**
   * Triggering.
   */
  void focusGame();
  void focusPage(PageType type);
  void unfocusPage();

  void tick(float delta);
};

/**
 * The main client app.
 * Its various components (the various pages and potential running game) are
 * neatly modularized; see client/elements/elements.h. This is simply the glue
 * that brings everything together.
 */
class Client : public ui::Control {
private:
  ClientShared shared;

  /**
   * Pages.
   */
  HomePage homePage;
  ListingPage listingPage;
  SettingsPage settingsPage;
  ui::Button backButton;

  /**
   * UI state.
   */
  ClientUIState uiState;
  std::vector<std::pair<sf::FloatRect, PageType>> pageRects;

  /**
   * Style settings.
   */
  const struct Style {
    const float unfocusedPageScale = 500.0f / 1600.0f,
        pageFocusAnimSpeed = 3, // s^-1
        gameFocusAnimSpeed = 4;

    const sf::Vector2f homeOffset{202.249, 479.047};
    const sf::Vector2f listingOffset{897.751, 479.047};
    const sf::Vector2f settingsOffset{550, 98.302};

    Style() { }
  } style;

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
  void unfocusPage();
  void focusPage(const PageType type);
  void beginGame(std::unique_ptr<Game> &&game);
};

int main();

#endif //SOLEMNSKY_CLIENT_H
