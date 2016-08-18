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
 * Connection management for the multiplayer client.
 */
#pragma once
#include "ui/control.hpp"
#include "client/engine/skyrender.hpp"
#include "util/telegraph.hpp"
#include "engine/event.hpp"
#include "engine/protocol.hpp"
#include "engine/debugview.hpp"
#include "client/elements/elements.hpp"
#include "client/elements/clientui.hpp"
#include "engine/flowcontrol.hpp"

/**
 * ArenaLogger proxy for MultiplayerCore, to intercept arena events for
 * the MultiplayerCore event log.
 */
class MultiplayerLogger: public sky::ArenaLogger {
 private:
  class MultiplayerCore &core;

 protected:
  void onEvent(const sky::ArenaEvent &event) override final;

 public:
  MultiplayerLogger(sky::Arena &arena, class MultiplayerCore &core);

};

/**
 * Subsystem proxy for MultiplayerCore, to intercept subsystem callbacks to
 * relay to ConnectionListener.
 */
class MultiplayerSubsystem: public sky::Subsystem<Nothing> {
 private:
  class MultiplayerCore &core;

 protected:
  void onMode(const sky::ArenaMode newMode) override final;
  void onStartGame() override final;
  void onEndGame() override final;

 public:
  MultiplayerSubsystem(sky::Arena &arena, class MultiplayerCore &core);

};

/**
 * State associated with a participation in an remote arena.
 * Instantiated by MultiplayerCore when the connection is active.
 */
struct ArenaConnection {
  ArenaConnection(
      const PID pid,
      const sky::ArenaInit &arenaInit,
      const sky::SkyHandleInit &skyHandleInit,
      const sky::ScoreboardInit &scoreboardInit);

  // Engine state.
  sky::Arena arena;
  sky::SkyHandle skyHandle;
  sky::Scoreboard scoreboard;
  sky::Player &player;
  SkyDeltaManager deltaManager;

  sky::DebugView debugView;

  // Handy accessors.
  sky::Sky *getSky();

};

/**
 * Entity that observes changes in the ArenaConnection.
 * Multiplayer has to do this.
 */
class ConnectionObserver {
 public:
  virtual void onConnect() { }
  virtual void onLoadMode(const sky::ArenaMode newMode) { }
  virtual void onStartGame() { }
  virtual void onEndGame() { }

};

/**
 * The core state of the multiplayer client, allocated for Multiplayer for
 * use by MultiplayerView.
 */
class MultiplayerCore: public ClientComponent {
  friend class MultiplayerLogger;
  friend class MultiplayerSubsystem;
 private:
  // Associated observer.
  ConnectionObserver &observer;

  // Connection state.
  optional<MultiplayerLogger> proxyLogger;
  optional<MultiplayerSubsystem> proxySubsystem;
  bool askedConnection;
  bool askedSky;
  Cooldown disconnectTimeout;

  tg::Telegraph<sky::ServerPacket> telegraph;
  tg::Host host;
  ENetPeer *server;
  bool disconnecting, // trying to disconnect
      disconnected; // it's over, close the multiplayer client

  // Callbacks from MultiplayerSubsystem.
  void onStartGame();
  void onEndGame();

  // Transmission timers.
  Scheduler participationUpdateSchedule;

  // Packet processing submethod.
  void processPacket(const sky::ServerPacket &packet);
  // (returns true when the queue has been exhausted)
  bool pollNetwork();

 public:
  MultiplayerCore(
      ClientShared &shared,
      ConnectionObserver &listener,
      const std::string &serverHostname,
      const unsigned short serverPort);
  ~MultiplayerCore();

  // MessageInteraction and printers.
  MessageInteraction messageInteraction;
  EnginePrinter enginePrinter;
  ClientPrinter clientPrinter;
  GameConsolePrinter consolePrinter;

  // Connection state.
  optional<ArenaConnection> conn;
  const tg::Host &getHost() const;
  bool isConnected() const;
  bool isDisconnecting() const;
  bool isDisconnected() const;

  // ClientComponent impl.
  void onChangeSettings(const ui::SettingsDelta &settings);

  // User API.
  void transmit(const sky::ClientPacket &packet);
  void disconnect();
  void poll();
  void tick(const TimeDiff delta);

  void chat(const std::string &message);
  void rcon(const std::string &command);
  void handleChatInput(const std::string &input);
  void requestTeamChange(const sky::Team team);
};

/**
 * A game interface we can offer the user.
 *
 * Throughout the course of a client's activity in a server, we may go through various
 * interfaces: the lobby, a loading screen, the game, the scoring screen, etc.
 */
class MultiplayerView: public ClientComponent, public ui::Control {
 public:
  // Shared state.
  MultiplayerCore &core;
  ArenaConnection &conn;

  MultiplayerView(
      ClientShared &shared,
      MultiplayerCore &core);

  // Callbacks.
  virtual void handleSkyAction(const sky::Action action, const bool state) { }
  virtual void handleClientAction(const ui::ClientAction action, const bool state) { }

};
