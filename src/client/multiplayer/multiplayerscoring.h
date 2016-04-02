/**
 * The various interfaces the multiplayer client can show.
 */
#pragma once
#include "multiplayershared.h"
#include "ui/widgets/widgets.h"

/**
 * At the scoring screen, we display the score and players talk about how
 * they were carrying their team.
 */
class MultiplayerScoring: public MultiplayerView {
 private:

 public:
  MultiplayerScoring(ClientShared &shared, MultiplayerConnection &connection);

  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
  void signalRead() override;
  void signalClear() override;

  void onChangeSettings(const SettingsDelta &settings) override;
};
