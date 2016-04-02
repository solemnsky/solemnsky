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
 * MultiplayerGame assumes connection.arena.sky exists. This is safe, but if
 * it's violated *bad things* happen.
 */

class MultiplayerGame: public MultiplayerView {
 private:
 public:
  MultiplayerGame(ClientShared &shared, MultiplayerConnection &connection);

  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
  void signalRead() override;
  void signalClear() override;

  void onChangeSettings(const SettingsDelta &settings) override;
};

