/**
 * Component of the multiplayer client corresponding to ArenaMode::Game.
 */
#pragma once
#include "multiplayershared.h"
#include "client/subsystem/render.h"
#include "client/ui/widgets/widgets.h"

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
  // sky and render subsystem
  sky::Sky &sky;
  sky::RenderSystem renderSystem;

  // state and stuff
  sf::Vector2f panning;
  Cooldown skyDeltaUpdate;

 public:
  MultiplayerGame(ClientShared &shared,
                  MultiplayerConnection &connection);

  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
  void signalRead() override;
  void signalClear() override;

  void onPacket(const sky::ServerPacket &packet) override;
  void onChangeSettings(const SettingsDelta &settings) override;
};

