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
  struct Style {
    sf::Vector2i scoreOverlayDims;
    sf::Vector2f chatPos,
        messageLogPos,
        readyButtonPos;
    float scoreOverlayTopMargin;
    float lobbyPlayersOffset,
        lobbyTopMargin,
        lobbyChatWidth,
        gameChatWidth;
    int lobbyFontSize;
    sf::Color playerSpecColor, playerJoinedColor;
    std::string readyButtonActiveDesc, readyButtonDeactiveDesc;
    Style();
  } style;

  /**
   * Shared state.
   */
  MultiplayerShared shared;

  /**
   * Graphics and UI.
   */

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
