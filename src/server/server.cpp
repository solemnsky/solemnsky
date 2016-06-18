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
#include "server.hpp"
#include "util/printer.hpp"

/**
 * ServerShared.
 */

ServerShared::ServerShared(
    tg::Host &host, tg::Telegraph<sky::ClientPacket> &telegraph,
    const sky::ArenaInit &arenaInit) :
    arena(arenaInit), // initialize engine state
    skyHandle(arena, {}),
    scoreboard(arena, {}),

    host(host),
    telegraph(telegraph) {}

sky::Player *ServerShared::playerFromPeer(ENetPeer *peer) const {
  if (peer->data) return (sky::Player *) peer->data;
  else return nullptr;
}

void ServerShared::registerArenaDelta(const sky::ArenaDelta &arenaDelta) {
  arena.applyDelta(arenaDelta);
  sendToClients(sky::ServerPacket::DeltaArena(arenaDelta));
}

void ServerShared::registerGameStart() {
  skyHandle.start();
  registerArenaDelta(sky::ArenaDelta::EnvLoadState(true));
}

void ServerShared::registerGameEnd() {
  skyHandle.stop();
  registerArenaDelta(sky::ArenaDelta::EnvLoadState(false));
}

void ServerShared::sendToClients(const sky::ServerPacket &packet,
                                 const bool guaranteeOrder) {
  telegraph.transmit(
      host,
      [&](std::function<void(ENetPeer *const)> transmit) {
        for (auto const peer : host.getPeers()) { transmit(peer); }
      }, packet, guaranteeOrder);
}

void ServerShared::sendToClientsExcept(const PID pid,
                                       const sky::ServerPacket &packet,
                                       const bool guaranteeOrder) {
  telegraph.transmit(
      host,
      [&](std::function<void(ENetPeer *const)> transmit) {
        for (auto const peer : host.getPeers()) {
          if (sky::Player *player = playerFromPeer(peer)) {
            if (player->pid != pid) transmit(peer);
          }
        }
      }, packet, guaranteeOrder);
}

void ServerShared::sendToClient(ENetPeer *const client,
                                const sky::ServerPacket &packet,
                                const bool guaranteeOrder) {
  telegraph.transmit(host, client, packet, guaranteeOrder);
}

void ServerShared::rconResponse(ENetPeer *const client,
                                const std::string &response) {
  sendToClient(client, sky::ServerPacket::RCon(response), true);
  logEvent(ServerEvent::RConOut(response));
}

void ServerShared::logEvent(const ServerEvent &event) {
  StringPrinter p;
  event.print(p);
  appLog(p.getString(), LogOrigin::Server);
}

void ServerShared::logArenaEvent(const sky::ArenaEvent &event) {
  StringPrinter p;
  event.print(p);
  appLog(p.getString(), LogOrigin::Engine);
}

/**
 * ServerLogger.
 */

void ServerLogger::onEvent(const sky::ArenaEvent &event) {
  shared.logArenaEvent(event);
}

ServerLogger::ServerLogger(ServerShared &shared, sky::Arena &arena) :
    sky::ArenaLogger(arena), shared(shared) {}

/**
 * ServerExec.
 */

void ServerExec::processPacket(ENetPeer *client,
                               const sky::ClientPacket &packet) {
  using namespace sky;

  if (Player *const player = shared.playerFromPeer(client)) {
    // the player is in the arena

    switch (packet.type) {
      case ClientPacket::Type::ReqSky: {
        if (auto sky = shared.skyHandle.getSky()) {
          shared.sendToClient(
              client, sky::ServerPacket::InitSky(sky->captureInitializer()));

          sky::PlayerDelta delta{*player};
          delta.loadingEnv = false;
          shared.registerArenaDelta(
              sky::ArenaDelta::Delta(player->pid, delta));
        }
      }

      case ClientPacket::Type::Pong: {
        latencyTracker.registerPong(*player,
                                    packet.pingTime.get(),
                                    packet.pongTime.get());
        break;
      }

      case ClientPacket::Type::ReqPlayerDelta: {
        const PlayerDelta &delta = packet.playerDelta.get();
        sky::PlayerDelta effectedDelta;
        if (delta.admin) {
          if (player->isAdmin()) effectedDelta.admin = delta.admin;
        }
        if (delta.nickname) {
          effectedDelta.nickname =
              shared.arena.allocNewNickname(*player, delta.nickname.get());
        }
        shared.registerArenaDelta(
            sky::ArenaDelta::Delta(
                player->pid, effectedDelta));
        break;
      }

      case ClientPacket::Type::ReqInput: {
        if (const auto sky = shared.skyHandle.getSky()) {
          sky->getParticipation(*player).applyInput(
              packet.participationInput.get());
        }
        if (shared.skyHandle.loadingToSky()) {
          appLog("Warning: received input from client while still loading "
                     "environment! Might want to define this case.");
        }
        break;
      }

      case ClientPacket::Type::Chat: {
        shared.sendToClients(sky::ServerPacket::Chat(
            player->pid, packet.stringData.get()));
        break;
      }

      case ClientPacket::Type::RCon: {
        shared.logEvent(ServerEvent::RConIn(packet.stringData.get()));
        break;
      }

      default:
        break;
    }

    server->onPacket(client, *player, packet);

  } else {
    // client hasn't joined the arena yet

    if (packet.type == ClientPacket::Type::ReqJoin) {
      const ArenaDelta delta =
          shared.arena.connectPlayer(packet.stringData.get());
      Player *newPlayer = shared.arena.getPlayer(delta.join->pid);
      client->data = newPlayer;

      shared.logEvent(ServerEvent::Connect(newPlayer->getNickname()));
      shared.sendToClient(client, ServerPacket::Init(
          newPlayer->pid,
          shared.arena.captureInitializer(),
          shared.skyHandle.captureInitializer(),
          shared.scoreboard.captureInitializer()), 0);
      shared.sendToClientsExcept(
          newPlayer->pid, ServerPacket::DeltaArena(delta));
    }
  }
}

bool ServerExec::poll() {
  // Network.
  static ENetEvent event;
  event = host.poll();

  switch (event.type) {
    case ENET_EVENT_TYPE_NONE:
      return true;
    case ENET_EVENT_TYPE_CONNECT: {
      appLog("Client connecting...", LogOrigin::Server);
      event.peer->data = nullptr;
      return false;
    }
    case ENET_EVENT_TYPE_DISCONNECT: {
      if (sky::Player *player = shared.playerFromPeer(event.peer)) {
        shared.logEvent(ServerEvent::Disconnect(player->getNickname()));
        shared.registerArenaDelta(sky::ArenaDelta::Quit(player->pid));
        event.peer->data = nullptr;
      }
      return false;
    }
    case ENET_EVENT_TYPE_RECEIVE: {
      if (const auto &reception = telegraph.receive(event.packet))
        processPacket(event.peer, *reception);
      return false;
    }
  }

  return true;
}

void ServerExec::tick(const TimeDiff delta) {
  // Environment loading.
  if (shared.skyHandle.readyToLoadSky()) {
    shared.skyHandle.instantiateSky({});
  }

  // Tick game state and network host.
  shared.arena.tick(delta);
  host.tick(delta);

  // SkyHandle updating.
  if (const auto handleDelta = shared.skyHandle.collectDelta()) {
    shared.sendToClients(sky::ServerPacket::DeltaSkyHandle(handleDelta.get()));
  }

  // Sky update scheduling.
  if (const auto sky = shared.skyHandle.getSky()) {
    if (skyDeltaTimer.cool(delta)) {
      auto skyDelta = sky->collectDelta();

      for (auto peer : host.getPeers()) {
        if (sky::Player *player = shared.playerFromPeer(peer)) {
          shared.sendToClient(
              peer, sky::ServerPacket::DeltaSky(
                  skyDelta.respectAuthority(*player),
                  shared.arena.getUptime()));
        } else {
          appLog("Warning: Peer attached to no client!"
                     " Think about what this means.");
        }
      }
      skyDeltaTimer.reset();
    }
  }

  // Scoreboard update scheduling.
  if (scoreDeltaTimer.cool(delta)) {
    if (const auto scoreDelta = shared.scoreboard.collectDelta()) {
      shared.sendToClients(
          sky::ServerPacket::DeltaScore(scoreDelta.get()));
    }
    scoreDeltaTimer.reset();
  }

  // Ping scheduling.
  if (pingTimer.cool(delta)) {
    shared.sendToClients(sky::ServerPacket::Ping(
        shared.arena.getUptime()));
    pingTimer.reset();
  }

  // Player latency update scheduling.
  if (latencyUpdateTimer.cool(delta)) {
    shared.registerArenaDelta(latencyTracker.makeUpdate());
    latencyUpdateTimer.reset();
  }
}

ServerExec::ServerExec(
    const Port port,
    const sky::ArenaInit &arenaInit,
    std::function<std::unique_ptr<ServerListener>(
        ServerShared &)> mkServer) :

    host(tg::HostType::Server, port),
    shared(host, telegraph, arenaInit),

    skyDeltaTimer(0.03),
    scoreDeltaTimer(0.5),
    pingTimer(1),
    latencyUpdateTimer(2),

    server(mkServer(shared)),

    logger(shared, shared.arena),
    latencyTracker(shared.arena),

    running(true) {
  shared.logEvent(ServerEvent::Start(port, arenaInit.name));
}

void ServerExec::run() {
  sf::Clock clock;
  while (running) {
    tick(clock.restart().asSeconds());
    while (!poll()) {}

    sf::sleep(sf::milliseconds(16));
  }
}
