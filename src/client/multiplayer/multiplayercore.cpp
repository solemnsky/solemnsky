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
#include "multiplayercore.hpp"
#include "client/elements/style.hpp"

/**
 * MultiplayerLogger.
 */

void MultiplayerLogger::onEvent(const sky::ArenaEvent &event) {
  core.enginePrinter.printLn(event);
}

MultiplayerLogger::MultiplayerLogger(sky::Arena &arena,
                                     class MultiplayerCore &core) :
    sky::ArenaLogger(arena),
    core(core) { }

/**
 * MultiplayerSubsystem.
 */

void MultiplayerSubsystem::onMode(const sky::ArenaMode newMode) {
  core.observer.onLoadMode(newMode);
}

void MultiplayerSubsystem::onStartGame() {
  core.onStartGame();
  core.observer.onStartGame();
}

void MultiplayerSubsystem::onEndGame() {
  core.onEndGame();
  core.observer.onEndGame();
}

MultiplayerSubsystem::MultiplayerSubsystem(sky::Arena &arena,
                                           class MultiplayerCore &core) :
    sky::Subsystem<Nothing>(arena),
    core(core) { }

/**
 * ArenaConnection.
 */

ArenaConnection::ArenaConnection(
    const PID pid,
    const sky::ArenaInit &arenaInit,
    const sky::SkyHandleInit &skyHandleInit,
    const sky::ScoreboardInit &scoreboardInit) :
    arena(arenaInit, {pid}),
    skyHandle(arena, skyHandleInit),
    scoreboard(arena, scoreboardInit),
    player(*arena.getPlayer(pid)),
    skyDeltaCache(arena, skyHandle),
    debugView(arena, skyHandle, pid) { }

sky::Sky *ArenaConnection::getSky() {
  return skyHandle.getSky();
}

/**
 * MultiplayerCore.
 */

void MultiplayerCore::effectDisconnection(const std::string &disconnectReason) {
  disconnected = true;
  this->disconnectReason = disconnectReason;
}

void MultiplayerCore::onStartGame() {

}

void MultiplayerCore::onEndGame() {
  askedSky = false;
}

void MultiplayerCore::processPacket(const sky::ServerPacket &packet) {
  using namespace sky;

  if (!conn) {
    // waiting for the arena connection request to be accepted
    if (packet.type == ServerPacket::Type::Init) {
      appLog("Loading arena...", LogOrigin::Client);
      conn.emplace(
          packet.pid.get(),
          packet.arenaInit.get(),
          packet.skyHandleInit.get(),
          packet.scoreInit.get());
      proxyLogger.emplace(conn->arena, *this);
      proxySubsystem.emplace(conn->arena, *this);
      appLog("Joined arena as " + inQuotes(conn->player.getNickname()), LogOrigin::Client);
      observer.onConnect();

      clientPrinter.printLn(ClientEvent::Connect(
          conn->arena.getName(),
          tg::printAddress(host.getPeers()[0]->address)));
    }
    return;
  }

  // we're in the arena, conn is instantiated
  switch (packet.type) {
    case ServerPacket::Type::InitSky: {
      auto &skyHandle = conn->skyHandle;
      if (skyHandle.getEnvironment() and
          skyHandle.getEnvironment()->getVisuals())
        skyHandle.instantiateSky(packet.skyInit.get());
      else {
        appLog("InitSky packet received before we have loaded! "
                  "The server is at fault and likely broken.", LogOrigin::Error);
      }
      break;
    }

    case ServerPacket::Type::Ping: {
      transmit(sky::ClientPacket::Pong(
          packet.timestamp.get(), conn->arena.getUptime()));
      break;
    }

    case ServerPacket::Type::DeltaArena: {
      conn->arena.applyDelta(packet.arenaDelta.get());
      break;
    }

    case ServerPacket::Type::DeltaSkyHandle: {
      conn->skyHandle.applyDelta(packet.skyHandleDelta.get());
      break;
    }

    case ServerPacket::Type::DeltaSky: {
      if (conn->skyHandle.getSky()) {
        conn->skyDeltaCache.receive(packet.timestamp.get(), packet.skyDelta.get());
      } else {
        appLog("Received sky delta packet before sky was initialized! "
                   "The server is at fault and is likely broken.", LogOrigin::Error);
      }
      break;
    }

    case ServerPacket::Type::DeltaScore: {
      conn->scoreboard.applyDelta(packet.scoreDelta.get());
      break;
    }

    case ServerPacket::Type::Chat: {
      if (sky::Player *chattyPlayer = conn->arena.getPlayer(
          packet.pid.get())) {
        clientPrinter.printLn(ClientEvent::Chat(
            chattyPlayer->getNickname(), packet.stringData.get()));
      }
      break;
    }

    case ServerPacket::Type::Broadcast: {
      clientPrinter.printLn(ClientEvent::Broadcast(packet.stringData.get()));
      break;
    }

    case ServerPacket::Type::RCon: {
      clientPrinter.printLn(ClientEvent::RConResponse(packet.stringData.get()));
      break;
    }

    default:
      break;
  }
}

bool MultiplayerCore::pollNetwork() {
  if (disconnected) return true;

  static ENetEvent event;
  event = host.poll();

  if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
    server = nullptr;
    appLog("Disconnected from server!", LogOrigin::Client);
    if (disconnecting) {
      effectDisconnection("Requested disconnection.");
    } else {
      effectDisconnection("Connection lost!");
    }
    return true;
  }

  if (!server) {
    if (event.type == ENET_EVENT_TYPE_CONNECT) {
      server = event.peer;
      appLog("Allocated connection!", LogOrigin::Client);
      return false;
    }
  } else {
    if (event.type == ENET_EVENT_TYPE_RECEIVE) {
      if (const auto reception = telegraph.receive(event.packet))
        processPacket(*reception);
    }
  }

  return event.type == ENET_EVENT_TYPE_NONE;
}

MultiplayerCore::MultiplayerCore(
    ClientShared &shared,
    ConnectionObserver &listener,
    const std::string &serverHostname,
    const unsigned short serverPort) :
    ClientComponent(shared),

    observer(listener),
    askedConnection(false),
    askedSky(false),
    disconnectTimer(1),

    host(tg::HostType::Client),
    server(nullptr),

    disconnecting(false),
    disconnected(false),

    participationUpdateSchedule(1.0f / 25.0f),

    messageInteraction(shared.references),
    enginePrinter(messageInteraction),
    clientPrinter(messageInteraction),
    consolePrinter(messageInteraction) {
  host.connect(serverHostname, serverPort);
}

MultiplayerCore::~MultiplayerCore() {
  proxyLogger.reset();
  proxySubsystem.reset(); // must destroy subsystem before its arena
}

const tg::Host &MultiplayerCore::getHost() const {
  return host;
}

bool MultiplayerCore::isConnected() const {
  return bool(server);
}

bool MultiplayerCore::isDisconnecting() const {
  return disconnecting;
}

bool MultiplayerCore::isDisconnected() const {
  return disconnected;
}

void MultiplayerCore::onChangeSettings(const ui::SettingsDelta &settings) {
  if (conn) {
    if (settings.nickname) {
      sky::PlayerDelta delta{conn->player};
      delta.nickname = *settings.nickname;
      transmit(sky::ClientPacket::ReqPlayerDelta(delta));
      // request a nickname change
    }
  }
}

void MultiplayerCore::transmit(const sky::ClientPacket &packet) {
  if (server) telegraph.transmit(host, server, packet);
}

void MultiplayerCore::disconnect() {
  if (server) {
    host.disconnect(server);
    disconnecting = true;
    disconnectTimer.reset();
  } else {
    effectDisconnection("Aborted connection to server.");
  }
}

void MultiplayerCore::poll() {
  if (disconnected) return;

  if (server && !askedConnection) {
    // we have a link but haven't sent an arena connection request
    appLog("Asking to join arena...", LogOrigin::Client);
    transmit(sky::ClientPacket::ReqJoin(shared.references.settings.nickname));
    askedConnection = true;
  }

  while (!pollNetwork()) { }

  if (conn) {
    conn->arena.poll();
  }
}

void MultiplayerCore::tick(const float delta) {
  host.tick(delta);;
  if (conn) {
    conn->arena.tick(delta);

    // Sending scheduled participation inputs.
    if (const auto &sky = conn->skyHandle.getSky()) {
      if (participationUpdateSchedule.tick(delta)) {
        const auto input = sky->getParticipation(conn->player).collectInput();
        if (input) {
          transmit(sky::ClientPacket::ReqInput(input.get(), conn->arena.getUptime()));
          participationUpdateSchedule.reset();
        }
      }
    }

    if (const auto &env = conn->skyHandle.getEnvironment()) {
      if (!conn->skyHandle.getSky()) {
        // We want to ask for the Sky.
        if (env->getMap() and env->getVisuals() and !askedSky) {
          transmit(sky::ClientPacket::ReqSky());
          askedSky = true;
        }

        // Loading secondary resources.
        if (env->loadingIdle() and !env->getVisuals()) {
          env->loadMore(true, false);
        }
      }

    }
  }

  if (disconnecting) {
    if (disconnectTimer.tick(delta)) {
      appLog("Disconnected from unresponsive server!", LogOrigin::Client);
      effectDisconnection("Disconnected from unresponsive server.");
    }
  }
}

void MultiplayerCore::chat(const std::string &message) {
  transmit(sky::ClientPacket::Chat(message));
}

void MultiplayerCore::rcon(const std::string &command) {
  clientPrinter.printLn(ClientEvent::RConCommand(command));
  transmit(sky::ClientPacket::RCon(command));
}

void MultiplayerCore::handleChatInput(const std::string &input) {
  if (input.size() >= 1) {
    if (input.size() > 1 and input[0] == '/') {
      rcon(input.substr(1));
    } else {
      chat(input);
    }
  }
}

void MultiplayerCore::requestTeamChange(const sky::Team team) {
  if (conn) transmit(sky::ClientPacket::ReqTeam(team));
}

std::string MultiplayerCore::getDisconnectReason() const {
  return disconnectReason;
}

/**
 * MultiplayerView.
 */

MultiplayerView::MultiplayerView(
    ClientShared &shared,
    MultiplayerCore &core) :
    ClientComponent(shared),
    ui::Control(shared.references),
    core(core),
    conn(core.conn.get()) { }

