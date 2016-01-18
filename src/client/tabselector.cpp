//
// Created by Chris on 1/18/2016.
//

#include "tabselector.h"

TabSelector::TabSelector() :
    deployState(0, 1, 1),
    cycleState(0, tabCount, 0) { }

void TabSelector::tick(float delta) {
  deployState += (deploying ? 1 : -1) * delta * (animSpeed / size);

  buttons.clear();
  for (int i = 0; i < tabCount; i++) {
    const float realWidth = deployState * size;
    buttons[(TabName) i] =
        {1600 - realWidth, i * size, realWidth, size};
  }
}

void TabSelector::render(ui::Frame &f) {
  const float realWidth = deployState * size;
  f.drawRect({1600 - realWidth, 0, realWidth, 900}, sf::Color::Red);

  for (auto &button : buttons) {
    switch (button.first) {
      case TabName::MainTab: {
        f.drawRect(button.second, sf::Color::Blue);
        break;
      }
      case TabName::SettingsTab: {
        f.drawRect(button.second, sf::Color::Blue);
        break;
      }
      case TabName::ListingTab: {
        f.drawRect(button.second, sf::Color::Blue);
        break;
      }
      case TabName::GameTab: {
        f.drawRect(button.second, sf::Color::Blue);
      }
    }
  }
}

void TabSelector::handle(const sf::Event &event) {
  if (event.MouseButtonPressed) {
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

void TabSelector::signalClear() {
  tabClick = {};
}
