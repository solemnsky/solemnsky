#ifndef SOLEMNSKY_CLIENT_H
#define SOLEMNSKY_CLIENT_H

#include "ui/ui.h"
#include "pageselector.h"
#include "game/tutorial.h"
#include "homepage.h"
#include "settingspage.h"
#include "listingpage.h"
#include "clientstate.h"

/**
 * The main client app.
 * Its various compontents (the various pages and potential running game) are
 * neatly modularized.
 */
class Client : public ui::Control {
private:
  ClientState state;

  /**
   * The various pages that make up the UI.
   */
  PageSelector pageSelector;
  HomePage homePage;
  ListingPage listingPage;
  SettingsPage settingsPage;

  /**
   * The game, which may or may not exist at a particular moment.
   */
  std::unique_ptr<Game> game;
  bool inGame(); // test if the game exists

  void startTutorial();

  /**
   * Misc helpers.
   */
  void drawPage(ui::Frame &f, const PageType type, ui::Control &page);
  ui::Control &referencePage(const PageType type);

public:
  Client();

  void tick(float delta) override;
  void render(ui::Frame &f) override;
  void handle(const sf::Event &event) override;

  virtual void signalRead() override;
  virtual void signalClear() override;
};

#endif //SOLEMNSKY_CLIENT_H
