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
  Cooldown pingCooldown;
  bool triedConnection; // have we sent a verbs requesting connection yet?
  bool disconnecting;
  Cooldown disconnectTimeout;
  ENetEvent event; // volatile

  /**
   * Arena.
   */
  sky::Player *myPlayer;
  sky::Arena arena;

  /**
   * Methods.
   */
  void transmitPacket(const sky::ClientPacket &packet);

};

/**
 * The interface for the client in a certain Arena mode.
 */
class MultiplayerMode: public ui::Control {
 private:
  MultiplayerShared &shared;

 public:
  MultiplayerMode(MultiplayerShared &shared);
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
