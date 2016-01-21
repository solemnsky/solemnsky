#ifndef SOLEMNSKY_CLIENT_H
#define SOLEMNSKY_CLIENT_H

#include "ui/ui.h"
#include "game/tutorial.h"
#include "homepage.h"
#include "settingspage.h"
#include "listingpage.h"
#include "clientstate.h"

enum class PageType {
  Home, Listing, Settings
};

/**
 * The main client app.
 * Its various components (the various pages and potential running game) are
 * neatly modularized.
 */
class Client : public ui::Control {
private:
  ClientState state;

  /**
   * Pages and page anim stuff.
   */
  HomePage homePage;
  ListingPage listingPage;
  SettingsPage settingsPage;
  PageType focusedPage;
  ui::Button backButton;
  bool pageFocused;
  Clamped focusAnim; // `elem` [0, 1], 1 is fully focused

  std::vector<std::pair<sf::FloatRect, PageType>> pageRects;
  // updated on render, for handling mouse events

  // visual values
  const float unfocusedPageScale = 500.0f / 1600.0f,
      pageFocusAnimSpeed = 3; // s^-1

  const sf::Vector2f homeOffset{202.249, 479.047};
  const sf::Vector2f listingOffset{897.751, 479.047};
  const sf::Vector2f settingsOffset{550, 98.302};

  /**
   * Misc helpers.
   */
  ui::Control &referencePage(const PageType type);
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
