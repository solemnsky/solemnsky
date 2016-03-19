/**
 * The various interfaces the multiplayer client can show.
 */
#pragma once
#include "multiplayershared.h"
#include "client/ui/widgets/widgets.h"
#include "client/subsystem/render.h"

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
  MultiplayerLobby(ClientShared &shared, MultiplayerConnection &connection);

  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
  void reset() override;
  void signalRead() override;
  void signalClear() override;

  void onPacket(const sky::ServerPacket &packet) override;
  void onChangeSettings(const SettingsDelta &settings) override;
};

/**
 * In the game, the rendered representation of the game is the central
 * point of the screen; peripheries include a chat / message interface, and a
 * score screen you can call up with tab.
 */

// subsystem listener for the MultiplayerGame
class MultiplayerGame;
class MultiplayerGameListener: public sky::Subsystem {
 private:
  MultiplayerGame &parent;

  virtual void tick(const float delta) override;
  virtual void addPlane(const sky::PID pid, sky::Plane &plane) override;
  virtual void removePlane(const sky::PID pid) override;
 public:
  MultiplayerGameListener(const Sky *sky, MultiplayerGame &parent);
};

class MultiplayerGame: public MultiplayerView {
 private:
  // sky and subsystems
  sky::Sky sky;
  sky::RenderSystem renderSystem;
  MultiplayerGameListener listener;

  // state and stuff
  sf::Vector2f panning;
  Cooldown skyDeltaUpdate;

 public:
  MultiplayerGame(ClientShared &shared, MultiplayerConnection &connection);

  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
  void signalRead() override;
  void signalClear() override;

  void onPacket(const sky::ServerPacket &packet) override;
  void onChangeSettings(const SettingsDelta &settings) override;
};

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

  void onPacket(const sky::ServerPacket &packet) override;
  void onChangeSettings(const SettingsDelta &settings) override;
};
