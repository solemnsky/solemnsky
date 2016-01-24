/**
 * Page where the user modifies settings.
 */
#ifndef SOLEMNSKY_SETTINGSPAGE_H
#define SOLEMNSKY_SETTINGSPAGE_H

#include "ui/control.h"
#include "clientstate.h"

#include "page.h"

class SettingsPage : public Page {
private:
  ClientState *state;

public:
  SettingsPage(ClientState *state);

  ui::Signal<ui::None> completeSignal; // user has completed entering settings

  /**
   * Page interface.
   */
  void reset() override;

  /**
   * Control interface.
   */
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  void handle(const sf::Event &event) override;

  void signalRead() override;
  void signalClear() override;
};

#endif //SOLEMNSKY_SETTINGSPAGE_H
