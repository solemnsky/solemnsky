/**
 * A tutorial to introduce new players to the mechanics.
 */
#pragma once
#include "client/elements/elements.h"
#include "sky/sky.h"
#include "client/skyrender.h"

class Tutorial : public Game {
private:
  sky::Arena arena;
  sky::Player *player;
  sky::SkyManager skyManager;
  sky::SkyRender skyRender;

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
