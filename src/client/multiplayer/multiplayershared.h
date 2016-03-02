/**
 * The shared core of the multiplayer client; the greatest common denominator
 * of all 3 client modes (the lobby, the game, and the scoring screen).
 */
#ifndef SOLEMNSKY_MULTIPLAYERSHARED_H
#define SOLEMNSKY_MULTIPLAYERSHARED_H

#include "client/ui/control.h"
#include "telegraph/telegraph.h"
#include "sky/arena.h"
#include "sky/protocol.h"
#include "client/elements/elements.h"

struct MultiplayerShared {
  MultiplayerShared(const std::string &serverHostname,
                    const unsigned short serverPort);
  /**
   * Connection state.
   */
  tg::Host host;
  ENetPeer *server;
  tg::Telegraph<sky::ServerPacket, sky::ClientPacket> telegraph;

  /**
   * Arena.
   */
  sky::Player *myPlayer;
  sky::Arena arena;

  /**
   * Methods.
   */
  void transmitServer(const sky::ClientPacket &packet);
};

/**
 * The interface for the client in a certain Arena mode; comes with style
 * tuning values.
 */
class MultiplayerMode: public ui::Control {
 protected:
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

  ClientShared &shared; // shared state from the client application
  MultiplayerShared &mShared;
  // shared state pertinant to the multiplayer system

 public:
  MultiplayerMode(ClientShared &shared, MultiplayerShared &mShared);
  virtual ~MultiplayerMode() { }

  virtual void onBlur() = 0;
  virtual void onFocus() = 0;
  virtual void onChangeSettings(const SettingsDelta &settings) = 0;
  virtual void doExit() = 0; // try to exit eventually

  /**
   * All the ui::Control virtual functions remain virtual.
   */
};

#endif //SOLEMNSKY_MULTIPLAYERSHARED_H
