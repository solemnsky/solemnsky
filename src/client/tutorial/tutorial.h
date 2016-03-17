/**
 * The
 */
#ifndef SOLEMNSKY_TUTORIAL_H
#define SOLEMNSKY_TUTORIAL_H

#include "client/elements/elements.h"
#include "sky/sky.h"
#include "client/subsystem/render.h"
#include "gamecontrols.h"

class Tutorial : public Game {
private:
  sky::Sky sky;
  sky::RenderSystem renderSystem;
  GameController controller;

  sky::Plane *plane;

public:
  Tutorial(ClientShared &state);

  /**
   * Game interface.
   */
  void onChangeSettings(const SettingsDelta &settings) override;
  void onBlur() override;
  void onFocus() override;
  void doExit() override;

  /**
   * Control interface.
   */
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
  void reset() override;
  void signalRead() override;
  void signalClear() override;
};

#endif //SOLEMNSKY_TUTORIAL_H
