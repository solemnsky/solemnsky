/**
 * Top-level multiplayer client, to play a game on a server.
 */
#ifndef SOLEMNSKY_MULTICLIENT_H
#define SOLEMNSKY_MULTICLIENT_H

#include "multiplayerui.h"

/**
 * Top-level multiplayer client.
 */
class MultiplayerClient: public Game {
 private:
  /**
   * Shared state and the currently active mode interface.
   */
  MultiplayerShared mShared;
  std::unique_ptr<MultiplayerMode> mode;

  /**
   * Networking stuff that we don't need to share with the mode.
   */
  Cooldown pingCooldown;
  ENetEvent netEvent;
  bool disconnecting;
  bool triedConnection; // have we sent a verbs requesting connection yet?
  bool disconnecting;
  Cooldown disconnectTimeout;

 public:
  MultiplayerClient(ClientShared &state,
                    const std::string &serverHostname,
                    const unsigned short serverPort);

  /**
   * Game interface.
   */
  void onBlur() override;
  void onFocus() override;
  void onChangeSettings(const SettingsDelta &settings) override;
  void doExit() override;

  /**
   * Control interface.
   */
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;

  void signalRead() override;
  void signalClear() override;
};

#endif //SOLEMNSKY_MULTICLIENT_H
