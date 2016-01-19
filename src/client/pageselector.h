/**
 *
 */
#ifndef SOLEMNSKY_TABSELECTOR_H
#define SOLEMNSKY_TABSELECTOR_H

#include "base/value.h"
#include "ui/ui.h"

enum class PageType {
  MainTab, SettingsTab, ListingTab, GameTab
};

const float tabCount = 4;

class PageSelector : public ui::Control {
private:
  const float animSpeed = 1000; // px/s
  const float size = 150;
  const float vSpace = 10;

  Clamped deployState;
  Cyclic cycleState;

  std::map<PageType, sf::FloatRect> buttons;

public:
  PageSelector();

  /**
   * Interface
   */
  bool deploying = false;

  /**
   * Signals
   */
  ui::Signal<PageType> tabClick;

  /**
   * Control implementation.
   */
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  void handle(const sf::Event &event) override;
  void signalClear() override;
};

#endif //SOLEMNSKY_TABSELECTOR_H
