#ifndef SOLEMNSKY_CLIENT_H
#define SOLEMNSKY_CLIENT_H

#include "ui/ui.h"
#include "game/tutorial.h"
#include "homepage.h"
#include "settingspage.h"
#include "listingpage.h"
#include "clientstate.h"

/**
 * The main client app.
 * Its various components (the various pages and potential running game) are
 * neatly modularized.
 */
class Client : public ui::Control {
private:
  /**
   * A bunch of state, accessible from sub-windows. Includes some crucial UI
   * state.
   */
  ClientState state;

  /**
   * Pages and a few other UI things. All of the interesting UI logic is in
   * ClientState, where it is a available from other places.
   */
  HomePage homePage;
  ListingPage listingPage;
  SettingsPage settingsPage;
  ui::Button backButton;

  Clamped focusAnim; // `elem` [0, 1], 1 is fully focused
  Clamped gameFocusAnim;

  std::vector<std::pair<sf::FloatRect, PageType>> pageRects;
  // updated on render, for handling mouse events

  // visual values
  const float unfocusedPageScale = 500.0f / 1600.0f,
      pageFocusAnimSpeed = 3, // s^-1
      gameFocusAnimSpeed = 4;

  const sf::Vector2f homeOffset{202.249, 479.047};
  const sf::Vector2f listingOffset{897.751, 479.047};
  const sf::Vector2f settingsOffset{550, 98.302};

  /**
   * Misc helpers.
   */
  Page &referencePage(const PageType type);
  void drawPage(ui::Frame &f, const PageType type,
                const sf::Vector2f &offset, const std::string &name,
                ui::Control &page);

public:
  Client();

  void attachState() override;

  void tick(float delta) override;
  void render(ui::Frame &f) override;
  void handle(const sf::Event &event) override;

  virtual void signalRead() override;
  virtual void signalClear() override;
};

#endif //SOLEMNSKY_CLIENT_H
