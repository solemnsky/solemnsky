/**
 * The multiplayer UI expressed in the form of MultiplayerSubmodes for the
 * arena lobby, game, and scoring screen.
 */
#ifndef SOLEMNSKY_MULTIPLAYERUI_H_H
#define SOLEMNSKY_MULTIPLAYERUI_H_H

#include "multiplayershared.h"
#include "sky/
#include "client/subsystem/render.h"

/**
 * In the lobby, we can talk to people, see teams, change teams, and vote for
 * the game to start.
 */
class MultiplayerLobby: public MultiplayerView {
 public:
  MultiplayerLobby(MultiplayerConnection &shared);

 private:
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
};

/**
 * In the game, the rendered representation of the game is the central
 * point of the screen; peripharies include a chat / message interface, and a
 * score screen you can call up with tab.
 */
class MultiplayerGame: public MultiplayerView {
 private:
  sky::RenderSystem renderSystem;

 public:
  MultiplayerGame(MultiplayerConnection &shared);

  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
};

/**
 * At the scoring screen, we display the score and players talk about how
 * they were carrying their team.
 */
class MultiplayerScoring: public MultiplayerView {
 private:

 public:
  MultiplayerScoring(MultiplayerConnection &shared);

  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
};

#endif //SOLEMNSKY_MULTIPLAYERUI_H_H
