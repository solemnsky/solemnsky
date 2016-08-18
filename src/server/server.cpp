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
      case ClientPacket::Type::ReqSky: {
        if (auto sky = shared.skyHandle.getSky()) {
          shared.sendToClient(
              client, sky::ServerPacket::InitSky(sky->captureInitializer()));

          sky::PlayerDelta delta{*player};
          delta.loadingEnv = false;
          shared.registerArenaDelta(
              sky::ArenaDelta::Delta(player->pid, delta));
        }
        break;
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
        inputManager.receive(*player, packet.timestamp.get(),
                             packet.participationInput.get());
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
          shared.scoreboard.captureInitializer()));
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
  if (!shared.skyHandle.getSky()) {
    if (auto *environment = shared.skyHandle.getEnvironment()) {
      if (environment->getMap() and environment->getMechanics()) {
        shared.skyHandle.instantiateSky({});
      } else {
        if (environment->loadingIdle() and !environment->loadingErrored()) {
          environment->loadMore(false, true);
        }
      }
    }
  }

  // Tick engine / subsystems forward.
  shared.arena.poll();
  shared.arena.tick(delta);
  // On the server there is no difference in the form that poll() and tick() are called.

  host.tick(delta);

  // SkyHandle updating.
  if (const auto handleDelta = shared.skyHandle.collectDelta()) {
    shared.sendToClients(sky::ServerPacket::DeltaSkyHandle(handleDelta.get()));
  }

  // Sky update scheduling.
  if (const auto sky = shared.skyHandle.getSky()) {

    if (skyDeltaSchedule.tick(delta)) {
      if (const auto skyDelta = sky->collectDelta()) {
        for (auto peer : host.getPeers()) {
          if (sky::Player *player = shared.playerFromPeer(peer)) {
            if (!player->isLoadingEnv())
              shared.sendToClient(
                  peer, sky::ServerPacket::DeltaSky(
                  skyDelta->respectAuthority(*player),
                  shared.arena.getUptime()));
          }
        }
        skyDeltaSchedule.reset();
      }
    }

  }

  // Scoreboard update scheduling.
  if (scoreDeltaSchedule.tick(delta)) {
    if (const auto scoreDelta = shared.scoreboard.collectDelta()) {
      shared.sendToClients(
          sky::ServerPacket::DeltaScore(scoreDelta.get()));
    }
    scoreDeltaSchedule.reset();
  }

  // Ping scheduling.
  if (pingSchedule.tick(delta)) {
    shared.sendToClients(sky::ServerPacket::Ping(
        shared.arena.getUptime()));
    pingSchedule.reset();
  }

  // Player latency update scheduling.
  if (latencyUpdateSchedule.tick(delta)) {
    shared.registerArenaDelta(latencyTracker.makeUpdate());
    latencyUpdateSchedule.reset();
  }
}

ServerExec::ServerExec(
    const Port port,
    const sky::ArenaInit &arenaInit,
    std::function<std::unique_ptr<ServerListener>(
        ServerShared &)> mkServer) :

    host(tg::HostType::Server, port),
    shared(host, telegraph, arenaInit),
    inputManager(shared),

    skyDeltaSchedule(1.0f / 25.0f),
    scoreDeltaSchedule(0.5f),
    pingSchedule(1),
    latencyUpdateSchedule(2),

    server(mkServer(shared)),

    logger(shared, shared.arena),
    latencyTracker(shared.arena),

    running(true) {

  time_t current;
  time(&current);
  std::srand(current);

  shared.logEvent(ServerEvent::Start(port, arenaInit.name));
}

void ServerExec::run() {
  sf::Clock clock;
  while (running) {
    tick(clock.restart().asSeconds());
    while (!poll()) { }

    sf::sleep(sf::milliseconds(16));
  }
}
