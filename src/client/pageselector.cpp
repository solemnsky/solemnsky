//
// Created by Chris on 1/18/2016.
//

#include "pageselector.h"

PageSelector::PageSelector() :
    deployState(0, 1, 1),
    cycleState(0, tabCount, 0) { }

void PageSelector::tick(float delta) {
  deployState += (deploying ? 1 : -1) * delta * (animSpeed / size);

  buttons.clear();
  for (int i = 0; i < tabCount; i++) {
    const float realWidth = deployState * size;
    buttons[(PageType) i] =
        {1600 - realWidth, i * size, realWidth, size};
  }
}

void PageSelector::render(ui::Frame &f) {
  const float realWidth = deployState * size;
  f.drawRect({1600 - realWidth, 0, realWidth, 900}, sf::Color::Red);

  for (auto &button : buttons) {
    switch (button.first) {
      case PageType::MainTab: {
        f.drawRect(button.second, sf::Color::Blue);
        break;
      }
      case PageType::SettingsTab: {
        f.drawRect(button.second, sf::Color::Blue);
        break;
      }
      case PageType::ListingTab: {
        f.drawRect(button.second, sf::Color::Blue);
        break;
      }
      case PageType::GameTab: {
        f.drawRect(button.second, sf::Color::Blue);
      }
    }
  }
}

void PageSelector::handle(const sf::Event &event) {
  if (event.MouseButtonReleased) {
    const sf::Vector2f clickPos =
        {(float) event.mouseButton.x, (float) event.mouseButton.y};
    for (auto &button : buttons) {
      if (button.second.contains(clickPos)) {
        tabClick.push_back(button.first);
        break;
      }
    }
  }
}

void PageSelector::signalClear() {
  tabClick = {};
}
