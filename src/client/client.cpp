#include "client.h"

Client::Client() :
    pageSelector() {
}

void Client::tick(float delta) {
  homePage.tick(delta);
  gamePage.tick(delta);
  settingsPage.tick(delta);
  listingPage.tick(delta);

  pageSelector.tick(delta);
}

ui::Control &Client::referencePage(const PageType type) {
  switch (type) {
    case PageType::HomePage:
      return homePage;
    case PageType::SettingsPage:
      return settingsPage;
    case PageType::ListingPage:
      return listingPage;
    case PageType::GamePage:
      return gamePage;
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
  drawPage(f, PageType::GamePage, gamePage);

  pageSelector.render(f);
}

void Client::handle(const sf::Event &event) {
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

void Client::signalRead() {
}

void Client::signalClear() {
  pageSelector.signalClear();
}
