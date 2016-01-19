#include "client.h"

bool Client::inGame() {
  if (game) return game->inGame;
  return false;
}

Client::Client() :
    pageSelector() {
}


void Client::startTutorial() {

}

ui::Control &Client::referencePage(const PageType type) {
  switch (type) {
    case PageType::HomePage:
      return homePage;
    case PageType::SettingsPage:
      return settingsPage;
    case PageType::ListingPage:
      return listingPage;
  }
}

void Client::drawPage(ui::Frame &f, const PageType type, ui::Control &page) {
  const float distance = cyclicDistance(pageSelector.cycleState, (float) type);
  if (distance < 1)
    f.withTransform(
        sf::Transform().translate(0, 1600 * distance),
        [&]() {
          page.render(f);
        });
}

void Client::render(ui::Frame &f) {
  drawPage(f, PageType::HomePage, homePage);
  drawPage(f, PageType::SettingsPage, settingsPage);
  drawPage(f, PageType::ListingPage, listingPage);

  pageSelector.render(f);
}

void Client::handle(const sf::Event &event) {
  if (inGame) {
    if (event.type == sf::Event::KeyPressed) {
      if (event.key.code == sf::Keyboard::Escape) {
        pageSelector.deploying = !pageSelector.deploying;
      }
    }
    pageSelector.handle(event);
    if (pageSelector.active) {
      referencePage(*pageSelector.active).handle(event);
    }
  }
}

void Client::tick(float delta) {
  if (inGame()) {
    game->tick(delta);
  } else {
    pageSelector.tick(delta);
    homePage.tick(delta);
    settingsPage.tick(delta);
    listingPage.tick(delta);
  }
}

void Client::signalRead() {
}

void Client::signalClear() {
  pageSelector.signalClear();
}

