/**
 *
 */
#ifndef SOLEMNSKY_TABSELECTOR_H
#define SOLEMNSKY_TABSELECTOR_H

#include "base/value.h"
#include "ui/ui.h"

enum class PageType {
  HomePage, SettingsPage, ListingPage, GamePage
};

const float tabCount = 4;

class PageSelector : public ui::Control {
private:
  const float deploySpeed = 2000, // px/s
      cycleSpeed = 8, // window/s
      size = 150, // px
      vSpace = 10; // px

  Clamped deployState;
  PageType aim;

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
  Cyclic cycleState;

  /**
   * Control implementation.
   */
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  void handle(const sf::Event &event) override;
  void signalClear() override;
};

#endif //SOLEMNSKY_TABSELECTOR_H
