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
#include "server.h"
#include "util/printer.h"

/**
 * ServerShared.
 */

ServerShared::ServerShared(
    tg::Host &host, tg::Telegraph <sky::ClientPacket> &telegraph,
    const sky::ArenaInit &arenaInit) :
    arena(arenaInit), skyHandle(arena, {}), scoreboard(arena, {}),
    host(host), telegraph(telegraph) { }

sky::Player *ServerShared::playerFromPeer(ENetPeer *peer) const {
  if (peer->data) return (sky::Player *) peer->data;
  else return nullptr;
}

void ServerShared::registerArenaDelta(const sky::ArenaDelta &arenaDelta) {
  arena.applyDelta(arenaDelta);
  sendToClients(sky::ServerPacket::DeltaArena(arenaDelta));
}

void ServerShared::sendToClients(const sky::ServerPacket &packet) {
  telegraph.transmit(
      host,
      [&]( std::function < void(ENetPeer *const)> transmit) {
        for (auto const peer : host.getPeers()) { transmit(peer); }
      }, packet);
      }

void ServerShared::sendToClientsExcept(const PID pid,
                                       const sky::ServerPacket &packet) {
  telegraph.transmit(
      host,
      [&]( std::function < void(ENetPeer *const)> transmit) {
        for (auto const peer : host.getPeers()) {
          if (sky::Player * player = playerFromPeer(peer)) {
            if (player->pid != pid) transmit(peer);
          }
        }
      }, packet);
      }

void ServerShared::sendToClient(ENetPeer *const client,
                                const sky::ServerPacket &packet) {
  telegraph.transmit(host, client, packet);
}

void ServerShared::rconResponse(ENetPeer *const client,
                                const std::string &response) {
  sendToClient(client, sky::ServerPacket::RCon(response));
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
    sky::ArenaLogger(arena), shared(shared) { }

/**
 * ServerExec.
 */

void ServerExec::processPacket(ENetPeer *client,
                               const sky::ClientPacket &packet) {
  using namespace sky;

  if (Player *const player = shared.playerFromPeer(client)) {
    // the player is in the arena

    switch (packet.type) {
      case ClientPacket::Type::ReqPlayerDelta: {
        const PlayerDelta &delta = packet.playerDelta.get();
        if (delta.admin && not player->isAdmin()) return;
        shared.registerArenaDelta(
            sky::ArenaDelta::Delta(
                player->pid, delta));
        break;
      }

      case ClientPacket::Type::ReqAction: {
        player->doAction(packet.action.get(), packet.state.get());
        break;
      }

      case ClientPacket::Type::Chat: {
        shared.sendToClients(sky::ServerPacket::Chat(
            player->pid, packet.stringData.get()));
        break;
      }

      case ClientPacket::Type::Pong: {
        latencyTracker.registerPong(*player, packet.pingTime, packet.pongTime);
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
      shared.sendToClient(
          client, ServerPacket::Init(
              newPlayer->pid,
              shared.arena.captureInitializer(),
              shared.skyHandle.captureInitializer(),
              shared.scoreboard.captureInitializer()));
      shared.sendToClientsExcept(
          newPlayer->pid, ServerPacket::DeltaArena(delta));
    }
  }
}

void ServerExec::poll(float delta) {
  uptime += delta;
  shared.arena.tick(delta);

  if (skyDeltaTimer.cool(delta)) {
    shared.sendToClients(sky::ServerPacket::DeltaSky(
        shared.skyHandle.collectDelta()));
    skyDeltaTimer.reset();
  }

  if (scoreDeltaTimer.cool(delta)) {
    if (const auto scoreDelta = shared.scoreboard.collectDelta()) {
      shared.sendToClients(
          sky::ServerPacket::DeltaScore(scoreDelta.get()));
    }
    scoreDeltaTimer.reset();
  }

  if (pingTimer.cool(delta)) {
    shared.sendToClients(sky::ServerPacket::Ping(
        shared.arena.getUptime()));
  }

  static ENetEvent event;
  event = host.poll(delta);
  switch (event.type) {
    case ENET_EVENT_TYPE_NONE:
      break;
    case ENET_EVENT_TYPE_CONNECT: {
      appLog("Client connecting...", LogOrigin::Server);
      event.peer->data = nullptr;
      break;
    }
    case ENET_EVENT_TYPE_DISCONNECT: {
      if (sky::Player * player = shared.playerFromPeer(event.peer)) {
        shared.logEvent(ServerEvent::Disconnect(player->getNickname()));
        shared.registerArenaDelta(sky::ArenaDelta::Quit(player->pid));
        event.peer->data = nullptr;
      }
      break;
    }
    case ENET_EVENT_TYPE_RECEIVE: {
      if (const auto &reception = telegraph.receive(event.packet))
        processPacket(event.peer, *reception);
      break;
    }
  }
}

ServerExec::ServerExec(
    const Port port,
    const sky::ArenaInit &arenaInit,
    std::function<std::unique_ptr<ServerListener>(
        ServerShared & )> mkServer) :
    uptime(0),

    host(tg::HostType::Server, port),
    shared(host, telegraph, arenaInit),

    skyDeltaTimer(0.03),
    scoreDeltaTimer(1),
    pingTimer(1),

    server(mkServer(shared)),

    logger(shared, shared.arena),
    latencyTracker(shared.arena),

    running(true) {
  shared.logEvent(ServerEvent::Start(port, arenaInit.name));
  scoreDeltaTimer.cool(0.5); // shift so it's out of sync with pings
}

void ServerExec::run() {
  sf::Clock clock;
  while (running) {
    poll(clock.restart().asSeconds());
    sf::sleep(sf::milliseconds(16));
  }
}
