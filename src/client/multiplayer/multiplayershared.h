/**
 * Where the multiplayer client converges.
 */
#pragma once
#include "ui/control.h"
#include "util/telegraph.h"
#include "sky/event.h"
#include "sky/arena.h"
#include "sky/protocol.h"
#include "client/elements/elements.h"

/**
 * Subsystem used by the MultiplayerConnection to listen to events.
 */
class MultiplayerSubsystem: public sky::Subsystem {
 private:
  class MultiplayerConnection &connection;

 protected:
  void registerPlayer(sky::Player &player) override;
  void unregisterPlayer(sky::Player &player) override;

  void onEvent(const sky::ArenaEvent &event) override;

 public:
  MultiplayerSubsystem(sky::Arena &arena,
                       class MultiplayerConnection &connection);
};

/**
 * A connection to a server; manages the enet connection state, follows the
 * protocol to maintain a sky::Arena up-to-date, and maintains a message log.
 */
class MultiplayerConnection {
 private:
  friend class MultiplayerSubsystem;

  ClientShared &shared;
  ENetEvent event;

  tg::Telegraph<sky::ServerPacket> telegraph;
  bool askedConnection;
  Cooldown disconnectTimeout;

  // process a packet, after we're connected via enet and have send a ReqJoin
  void processPacket(const sky::ServerPacket &packet);

 public:
  void logEvent(const ClientEvent &event);
  void logArenaEvent(const sky::ArenaEvent &event);

  MultiplayerConnection(
      ClientShared &shared,
      const std::string &serverHostname,
      const unsigned short serverPort);

  /**
   * Connection state.
   */
  tg::Host host;
  ENetPeer *server;
  bool disconnecting, // trying to disconnect
      disconnected; // it's over, close the multiplayer client

  /**
   * Arena and subsystems.
   */
  void initializeArena(
      const PID pid,
      const sky::ArenaInitializer &arenaInit,
      const sky::SkyInitializer &skyInit);
  optional<sky::Arena> arena;
  optional<sky::Sky> sky;
  optional<MultiplayerSubsystem> multiplayerSubsystem;
  sky::Player *myPlayer;
  std::vector<ClientEvent> eventLog;

  /**
   * Methods.
   */
  void transmit(const sky::ClientPacket &packet);
  void poll(const float delta);
  void disconnect();

  void requestTeamChange(const sky::Team team);
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

  virtual void onChangeSettings(const SettingsDelta &settings) = 0;
};
