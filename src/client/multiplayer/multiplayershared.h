/**
 * solemnsky: the open-source multiplayer competitive 2D plane game
 * Copyright (C) 2016  Chris Gadzinski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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
 * Logger used by MultiplayerShared to listen to ArenaEvents.
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
 * Game state associated with a participation in an remote arena.
 */
struct ArenaConnection {
  ArenaConnection(
      MultiplayerShared &shared,
      const PID pid,
      const sky::ArenaInit &arenaInit,
      const sky::SkyHandleInitializer &skyInit);

  // State.
  sky::Arena arena;
  sky::SkyHandle skyManager;
  sky::Player &player;

  MultiplayerLogger logger;

  // Handy accessors.
  const optional<sky::Sky> &getSky() const;

};

/**
 * The state of the multiplayer client, allocated for Multiplayer for use by
 * MultiplayerView.
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

  // connection state
  tg::Host host;
  ENetPeer *server;
  bool disconnecting, // trying to disconnect
      disconnected; // it's over, close the multiplayer client
  optional<ArenaConnection> conn;

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
class MultiplayerView: public ClientComponent, public ui::Control {
 public:
  // Shared state.
  MultiplayerShared &mShared;
  ArenaConnection &conn; // from mShared

  MultiplayerView(
      sky::ArenaMode target,
      ClientShared &shared,
      MultiplayerShared &mShared);
  virtual ~MultiplayerView() { }

  const sky::ArenaMode target;

};
