/**
 * Top-level client, designed to connect the end-user to the Sky efficiently.
 */
#ifndef SOLEMNSKY_CLIENT_H
#define SOLEMNSKY_CLIENT_H

#include "game/tutorial.h"
#include "homepage.h"
#include "settingspage.h"
#include "listingpage.h"

class ClientUiState {
private:
  const float pageFocusAnimSpeed = 3, // s^-1
      gameFocusAnimSpeed = 4;
public:
  ClientUiState();

  PageType focusedPage;
  bool pageFocusing;
  bool gameFocusing;

  Clamped pageFocusFactor;
  Clamped gameFocusFactor;

  bool pageFocused();
  bool menuFocused();
  bool gameFocused();

  /**
   * Triggering.
   */
  void focusGame();
  void unfocusGame();
  void focusPage(PageType page);
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
  ClientUiState uiState;
  std::vector<std::pair<sf::FloatRect, PageType>> pageRects;

  /**
   * Style settings.
   */
  const struct Style {
    const float unfocusedPageScale = 500.0f / 1600.0f;

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
  void beginGame(std::unique_ptr<Game> &&game);

  void focusGame();
  void unfocusGame();

  void focusPage(const PageType type);
  void unfocusPage();
};

int main();

#endif //SOLEMNSKY_CLIENT_H
