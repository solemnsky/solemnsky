#include "client.h"


Client::Client() :
    tabSelector() {
}

void Client::tick(float delta) {
  tabSelector.tick(delta);
}

void Client::render(ui::Frame &f) {
  tabSelector.render(f);
}

void Client::handle(const sf::Event &event) {
  if (event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::Escape) {
      tabSelector.deploying = !tabSelector.deploying;
    }
  }
  tabSelector.handle(event);
}

void Client::signalRead() {
  for (const auto tabClick : tabSelector.tabClick) {
    appLog(LogType::Debug, "clicked tab " + std::to_string((int) tabClick));
  }
}

void Client::signalClear() {
  tabSelector.signalClear();
}
