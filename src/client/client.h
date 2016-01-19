#ifndef SOLEMNSKY_CLIENT_H
#define SOLEMNSKY_CLIENT_H

#include "ui/ui.h"
#include "pageselector.h"
#include "gamepage.h"
#include "homepage.h"
#include "settingspage.h"
#include "listingpage.h"
#include "clientstate.h"

/**
 * The main client app. This is going to get pretty huge fast, our luck is that
 * the UI can be neatly modularized.
 */
class Client : public ui::Control {
private:

  ClientState state;

  /**
   * The various pages...
   */
  HomePage homePage;
  GamePage gamePage;
  SettingsPage settingsPage;
  ListingPage listingPage;
  PageSelector pageSelector;

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
