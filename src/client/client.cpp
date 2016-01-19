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

void Client::render(ui::Frame &f) {
  pageSelector.render(f);
}

void Client::handle(const sf::Event &event) {
  if (event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::Escape) {
      pageSelector.deploying = !pageSelector.deploying;
    }
  }
  pageSelector.handle(event);
}

void Client::signalRead() {
  for (const auto tabClick : pageSelector.tabClick) {
    appLog(LogType::Debug, "clicked tab " + std::to_string((int) tabClick));
  }
}

void Client::signalClear() {
  pageSelector.signalClear();
}
