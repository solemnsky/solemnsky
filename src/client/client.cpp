#include "client.h"

bool Client::inGame() {
  if (game) return game->inFocus;
  return false;
}

Client::Client() :
    state(),
    pageSelector(),
    homePage(&state),
    listingPage(&state),
    settingsPage(&state) { }


void Client::startTutorial() {
  game = std::make_unique<Tutorial>(&state);
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
  bool drawPages = true;
  float pageAlpha = 1;

  if (game) {
    game->render(f);
    if (game->inFocus) {
      drawPages = false;
    } else {
      drawPages = true;
      pageAlpha = 0.5;
    }
  }

  if (drawPages) {
    f.withAlpha(pageAlpha, [&]() {
      drawPage(f, PageType::HomePage, homePage);
      drawPage(f, PageType::SettingsPage, settingsPage);
      drawPage(f, PageType::ListingPage, listingPage);

      pageSelector.render(f);
    });
  }
}

void Client::handle(const sf::Event &event) {
  if (game) {
    game->handle(event);
  } else {
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
  state.uptime += delta;

  if (game) {
    game->tick(delta);
  }

  pageSelector.tick(delta);
  homePage.tick(delta);
  settingsPage.tick(delta);
  listingPage.tick(delta);
}

void Client::signalRead() {
  if (!pageSelector.clicked.empty()) {
    pageSelector.deploying = false;
  }
}

void Client::signalClear() {
  pageSelector.signalClear();
}

