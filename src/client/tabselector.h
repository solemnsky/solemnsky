/**
 *
 */
#ifndef SOLEMNSKY_TABSELECTOR_H
#define SOLEMNSKY_TABSELECTOR_H

#include "base/value.h"
#include "ui/ui.h"

enum class TabName {
  MainTab, SettingsTab, ListingTab, GameTab
};

const float tabCount = 4;

class TabSelector : public ui::Control {
private:
  const float animSpeed = 1000; // px/s
  const float size = 150;
  const float vSpace = 10;

  Clamped deployState;
  Cyclic cycleState;

  std::map<TabName, sf::FloatRect> buttons;

public:
  TabSelector();

  /**
   * Interface
   */
  bool deploying = false;

  /**
   * Signals
   */
  ui::Signal<TabName> tabClick;

  /**
   * Control implementation.
   */
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  void handle(const sf::Event &event) override;
  void signalClear() override;
};

#endif //SOLEMNSKY_TABSELECTOR_H
