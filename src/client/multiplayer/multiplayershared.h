/**
 * Where the multiplayer client converges.
 */
#pragma once
#include "ui/control.h"
#include "client/skyrender.h"
#include "util/telegraph.h"
#include "sky/event.h"
#include "sky/arena.h"
#include "sky/protocol.h"
#include "client/elements/elements.h"

/**
 * Subsystem used by the MultiplayerConnection to listen to events.
 */
class MultiplayerLogger: public sky::ArenaLogger {
 private:
  class MultiplayerShared &connection;

 protected:
  void onEvent(const sky::ArenaEvent &event) override;

 public:
  MultiplayerLogger(sky::Arena &arena,
                    class MultiplayerShared &connection);
};

/**
 * The shared state of the multiplayer client, at the intersection of the
 * three multiplayer views. Holds the Arena and friends, Manages the basic
 * protocol, and provides utilities.
 */
class MultiplayerShared {
 private:
  friend class MultiplayerLogger;

  ClientShared &shared;
  ENetEvent event;

  tg::Telegraph<sky::ServerPacket> telegraph;
  bool askedConnection;
  Cooldown disconnectTimeout;

  // process a packet, after we're connected via enet and have send a ReqJoin
  void processPacket(const sky::ServerPacket &packet);

 public:
  MultiplayerShared(
      ClientShared &shared,
      const std::string &serverHostname,
      const unsigned short serverPort);

  // event logging
  void logEvent(const ClientEvent &event);
  void logArenaEvent(const sky::ArenaEvent &event);
  std::vector<ClientEvent> eventLog;

  //
  tg::Host host;
  ENetPeer *server;
  bool disconnecting, // trying to disconnect
      disconnected; // it's over, close the multiplayer client

  // arena and subsystems
  void initializeArena(
      const PID pid,
      const sky::ArenaInitializer &arenaInit,
      const sky::SkyInitializer &skyInit);
  optional<sky::Arena> arena;
  optional<sky::Sky> sky;
  optional<sky::SkyRender> skyRender;
  optional<MultiplayerLogger> logger;
  sky::Player *player;
  sky::Plane *plane;

  /**
   * Additional methods for Multiplayer.
   */
  void transmit(const sky::ClientPacket &packet);
  void poll(const float delta);
  void disconnect();
  void onChangeSettings(const SettingsDelta &settings);

  /**
   * Additional methods for MultiplayerView.
   */
  void chat(const std::string &message);
  void rcon(const std::string &command);
  void handleChatInput(const std::string &input);

  void requestTeamChange(const sky::Team team);

  void drawEventLog(ui::Frame &f, const float cutoff);

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
  MultiplayerShared &mShared;

  MultiplayerView(
      sky::ArenaMode target,
      ClientShared &shared,
      MultiplayerShared &mShared);
  virtual ~MultiplayerView() { }

  const sky::ArenaMode target; // the target of this view

  virtual void onChangeSettings(const SettingsDelta &settings) = 0;
};
