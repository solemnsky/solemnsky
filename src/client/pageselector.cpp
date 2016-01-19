//
// Created by Chris on 1/18/2016.
//

#include "pageselector.h"

PageSelector::PageSelector() :
    deployState(0, 1, 1),
    cycleState(0, tabCount, 0) { }

void PageSelector::tick(float delta) {
  deployState += (deploying ? 1 : -1) * delta * (animSpeed / size);

  const float yOffset = 450 - (tabCount * (size + vSpace) / 2);

  buttons.clear();
  for (int i = 0; i < tabCount; i++) {
    const float realWidth = deployState * size;
    buttons[(PageType) i] =
        {1600 - realWidth, yOffset + i * (size + vSpace), realWidth, size};
  }
}

void PageSelector::render(ui::Frame &f) {
  const float realWidth = deployState * size;
  f.drawSprite(textureOf(Res::PageSelector),
               {1600 - realWidth, 0},
               {0, 0, 150, 900});

  {
    const auto &button = buttons[PageType::MainTab];
    f.drawSprite(textureOf(Res::MainMenuIcon), {button.left, button.top},
                 {0, 0, 150, 150});
  }
  {
    const auto &button = buttons[PageType::SettingsTab];
    f.drawSprite(textureOf(Res::MainMenuIcon), {button.left, button.top},
                 {0, 0, 150, 150});
  }
  {
    const auto &button = buttons[PageType::ListingTab];
    f.drawSprite(textureOf(Res::MainMenuIcon), {button.left, button.top},
                 {0, 0, 150, 150});
  }
  {
    const auto &button = buttons[PageType::GameTab];
    f.drawSprite(textureOf(Res::GameIcon), {button.left, button.top},
                 {0, 0, 150, 150});
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
