/**
 * Where the multiplayer client converges.
 */
#pragma once
#include "client/ui/control.h"
#include "client/elements/event.h"
#include "util/telegraph.h"
#include "sky/arena.h"
#include "sky/protocol.h"
#include "client/elements/elements.h"

/**
 * A connection to a server; manages the enet connection state, follows the
 * protocol to maintain a sky::Arena up-to-date, and maintains a message log.
 */
class MultiplayerConnection {
 private:
  ClientShared &shared;

  ENetEvent event;

  tg::Telegraph<sky::ServerPacket> telegraph;
  bool askedConnection;
  Cooldown disconnectTimeout;

  void processPacket(const sky::ServerPacket &packet);

 public:
  MultiplayerConnection(
      ClientShared &shared,
      const std::string &serverHostname,
      const unsigned short serverPort);

  /**
   * Connection state.
   */
  tg::Host host;
  ENetPeer *server;
  bool connected; // connected to the arena
  bool disconnected, disconnecting;

  /**
   * Game state.
   */
  sky::Player *myPlayer;
  optional<sky::Arena> arena;
  std::vector<ClientEvent> eventLog;

  /**
   * Methods.
   */
  void transmit(const sky::ClientPacket &packet);
  optional<sky::ServerPacket> poll(const float delta);
  void disconnect();

  void requestTeamChange(const Team team);
};

/**
 * The interface for the client in a certain Arena mode; given that the
 * interface is modal, we should write it in modes.
 *
 * These classes act assuming that the connection is active
 * (conneciton.myPlayer is not null).
 */
class MultiplayerView: public ui::Control {
 public:
  // shared state
  ClientShared &shared;
  MultiplayerConnection &connection;

  MultiplayerView(
      sky::ArenaMode target,
      ClientShared &shared,
      MultiplayerConnection &connection);
  virtual ~MultiplayerView() { }

  const sky::ArenaMode target; // the target of this view

  virtual void onPacket(const sky::ServerPacket &packet) = 0;
  virtual void onChangeSettings(const SettingsDelta &settings) = 0;
};
