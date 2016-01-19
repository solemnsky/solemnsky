/**
 * Datatype with settings / UI for setting them / methods to read and write
 * to a settings file.
 */
#ifndef SOLEMNSKY_SETTINGS_H
#define SOLEMNSKY_SETTINGS_H

#include "ui/ui.h"

class SettingsPage : public ui::Control {
public:
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  void handle(const sf::Event &event) override;

  ui::Signal<ui::None> completeSignal; // user has completed entering settings

  void signalRead() override;
  void signalClear() override;
};

#endif //SOLEMNSKY_SETTINGS_H
