/**
 * Page where the user modifies settings.
 */
#ifndef SOLEMNSKY_SETTINGSPAGE_H
#define SOLEMNSKY_SETTINGSPAGE_H

#include "elements/elements.h"

class SettingsPage : public Page {
public:
  SettingsPage(ClientShared &state);

  bool completeSignal; // user has completed entering settings

  /**
   * Page interface.
   */
  void reset() override;

  /**
   * Control interface.
   */
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;

  void signalRead() override;
  void signalClear() override;
};

#endif //SOLEMNSKY_SETTINGSPAGE_H
