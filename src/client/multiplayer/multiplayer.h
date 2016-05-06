/**
 * Play a game on a server.
 */
#pragma once
#include "multiplayershared.h"

/**
 * Top-level multiplayer client.
 */
class Multiplayer: public Game {
 private:
  /**
   * Arena connection state / protocol implementation and the current
   * (mode-specific) interface that we show the user.
   */
  MultiplayerShared mShared;
  std::unique_ptr<MultiplayerView> view;
  std::unique_ptr<MultiplayerView> mkView();

 public:
  Multiplayer(ClientShared &shared,
              const std::string &serverHostname,
              const unsigned short serverPort);

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
