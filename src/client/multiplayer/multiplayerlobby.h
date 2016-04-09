/**
 * Component of the multiplayer client corresponding to ArenaMode::Lobby.
 */
#pragma once
#include "multiplayershared.h"
#include "ui/widgets/widgets.h"

/**
 * In the lobby, we can talk to people, see teams, change teams, and vote for
 * the game to start.
 */

class MultiplayerLobby: public MultiplayerView {
 private:
  ui::Button specButton;
  ui::Button redButton;
  ui::Button blueButton;
  ui::TextEntry chatInput;

  std::vector<ui::Control *> myControls;

 public:
  MultiplayerLobby(ClientShared &shared, MultiplayerShared &connection);

  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
  void reset() override;
  void signalRead() override;
  void signalClear() override;

  void onChangeSettings(const SettingsDelta &settings) override;
};



