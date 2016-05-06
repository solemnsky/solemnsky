/**
 * Component of the multiplayer client corresponding to ArenaMode::Game.
 */
#pragma once
#include "multiplayershared.h"
#include "client/skyrender.h"
#include "ui/widgets/widgets.h"

/**
 * In the game, the rendered representation of the game is the central
 * point of the screen; peripheries include a chat / message interface, and a
 * score screen you can call up with tab.
 *
 * Invariant: conn->skyManager
 */
class MultiplayerGame: public MultiplayerView {
 private:
  ui::TextEntry chatInput;
  bool scoreboardFocused;
  sky::SkyRender skyRender;
  sky::Participation participation;

  void doClientAction(const ClientAction action, const bool state);

 public:
  MultiplayerGame(ClientShared &shared, MultiplayerShared &connection);

  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
  void signalRead() override;
  void signalClear() override;

  void onChangeSettings(const SettingsDelta &settings) override;
};

