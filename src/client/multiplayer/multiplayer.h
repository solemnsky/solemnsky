/**
 * Top-level multiplayer client, to play a game on a server.
 */
#ifndef SOLEMNSKY_MULTICLIENT_H
#define SOLEMNSKY_MULTICLIENT_H

#include "multiplayerui.h"

/**
 * Top-level multiplayer client.
 */
class Multiplayer: public Game {
 private:
  /**
   * Arena connection state / protocol implementation and the current
   * (mode-specific) interface that we show the user.
   */
  MultiplayerConnection connection;
  std::unique_ptr<MultiplayerView> view;
  void switchView();

 public:
  Multiplayer(ClientShared &shared,
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
