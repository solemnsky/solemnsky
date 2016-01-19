/**
 * Page where the user modifies settings.
 */
#ifndef SOLEMNSKY_SETTINGS_H
#define SOLEMNSKY_SETTINGS_H

#include "ui/ui.h"
#include "clientstate.h"

class SettingsPage : public ui::Control {
private:
  ClientState *state;

public:
  SettingsPage(ClientState *state);

  ui::Signal<ui::None> completeSignal; // user has completed entering settings

  /**
   * Control interface.
   */
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  void handle(const sf::Event &event) override;


  void signalRead() override;
  void signalClear() override;
};

#endif //SOLEMNSKY_SETTINGS_H
