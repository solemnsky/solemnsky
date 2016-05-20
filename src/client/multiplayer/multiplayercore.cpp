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
#include "multiplayershared.h"
#include "client/elements/style.h"
#include "sky/event.h"
#include "util/printer.h"

/**
 * MultiplayerLogger.
 */

void MultiplayerLogger::onEvent(const sky::ArenaEvent &event) {
  connection.logArenaEvent(event);
}

MultiplayerLogger::MultiplayerLogger(sky::Arena &arena,
                                     MultiplayerCore &connection) :
    sky::ArenaLogger(arena), connection(connection) { }

/**
 * ArenaConnection.
 */

ArenaConnection::ArenaConnection(
    MultiplayerCore &shared,
    const PID pid,
    const sky::ArenaInit &arenaInit,
    const sky::SkyHandleInitializer &skyInit) :
    arena(arenaInit),
    skyHandle(arena, skyInit),
    player(*arena.getPlayer(pid)),
    logger(arena, shared) { }

const optional<sky::Sky> &ArenaConnection::getSky() const {
  return skyHandle.getSky();
}

/**
 * MultiplayerConnection.
 */

void MultiplayerCore::processPacket(const sky::ServerPacket &packet) {
  using namespace sky;

  if (!conn) {
    // waiting for the arena connection request to be accepted
    if (packet.type == ServerPacket::Type::Init) {
      appLog("Loading arena...", LogOrigin::Client);
      conn.emplace(
          *this,
          packet.pid.get(),
          packet.arenaInit.get(),
          packet.skyInit.get());
      appLog("Joined arena!", LogOrigin::Client);

      logEvent(ClientEvent::Connect(
          conn->arena.getName(),
          tg::printAddress(host.getPeers()[0]->address)));
    }
    return;
  }

  // we're in the arena
  switch (packet.type) {
    case ServerPacket::Type::Pong: {
      break;
    }

    case ServerPacket::Type::DeltaArena: {
      conn->arena.applyDelta(packet.arenaDelta.get());
      break;
    }

    case ServerPacket::Type::DeltaSky: {
      conn->skyHandle.applyDelta(packet.skyDelta.get());
      break;
    }

    case ServerPacket::Type::Chat: {
      if (sky::Player *chattyPlayer = conn->arena.getPlayer(
          packet.pid.get())) {
        logEvent(ClientEvent::Chat(
            chattyPlayer->getNickname(), packet.stringData.get()));
      }
      break;
    }

    case ServerPacket::Type::Broadcast: {
      logEvent(ClientEvent::Broadcast(packet.stringData.get()));
      break;
    }

    case ServerPacket::Type::RCon: {
      logEvent(ClientEvent::RConResponse(packet.stringData.get()));
      break;
    }

    default:
      break;
  }
}

MultiplayerCore::MultiplayerCore(
    ClientShared &shared,
    ConnectionListener &listener,
    const std::string &serverHostname,
    const unsigned short serverPort) :
    shared(shared),
    listener(listener),
    askedConnection(false),
    disconnectTimeout(1),

    host(tg::HostType::Client),
    server(nullptr),

    disconnecting(false), disconnected(false) {
  host.connect(serverHostname, serverPort);
}

void MultiplayerCore::logEvent(const ClientEvent &event) {
  StringPrinter p;
  event.print(p);
  appLog(p.getString(), LogOrigin::Client);
  eventLog.push_back(event);
}

void MultiplayerCore::logArenaEvent(const sky::ArenaEvent &event) {
  StringPrinter p;
  event.print(p);
  appLog(p.getString(), LogOrigin::Engine);
  eventLog.push_back(ClientEvent::Event(event));
}

void MultiplayerCore::drawEventLog(ui::Frame &f, const float cutoff) {
  f.drawText(
      style.multi.messageLogPos, [&](ui::TextFrame &tf) {
        for (auto iter = eventLog.rbegin();
             iter < eventLog.rend(); iter++) {
          ClientEvent(*iter).print(tf);
          tf.breakLine();
          if (tf.drawOffset.y < -cutoff) break;
        }
      }, style.multi.messageLogText);
}

void MultiplayerCore::transmit(const sky::ClientPacket &packet) {
  if (server) telegraph.transmit(host, server, packet);
}

void MultiplayerCore::poll(const float delta) {
  if (disconnected) return;

  event = host.poll();
  if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
    server = nullptr;
    appLog("Disconnected from server!", LogOrigin::Client);
    disconnected = true;
    return;
  }

  if (disconnecting) {
    if (disconnectTimeout.cool(delta)) {
      appLog("Disconnected from unresponsive server!", LogOrigin::Client);
      disconnected = true;
    }
    return;
  }

  if (!server) {
    // still trying to connect to the server...
    if (event.type == ENET_EVENT_TYPE_CONNECT) {
      server = event.peer;
      appLog("Allocated connection!", LogOrigin::Client);
    }
  } else {
    // connected
    if (!askedConnection) {
      // we have a link but haven't sent an arena connection request
      appLog("Asking to join arena...", LogOrigin::Client);
      transmit(sky::ClientPacket::ReqJoin(shared.settings.nickname));
      askedConnection = true;
      return;
    }

    if (event.type == ENET_EVENT_TYPE_RECEIVE) {
      if (const auto reception = telegraph.receive(event.packet))
        processPacket(*reception);
    }
  }
}

void MultiplayerCore::disconnect() {
  if (server) {
    host.disconnect(server);
    disconnecting = true;
    disconnectTimeout.reset();
  } else {
    disconnected = true;
  }
}

void MultiplayerCore::chat(const std::string &message) {
  transmit(sky::ClientPacket::Chat(message));
}

void MultiplayerCore::rcon(const std::string &command) {
  logEvent(ClientEvent::RConCommand(command));
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
  if (conn) {
    sky::PlayerDelta delta = conn->player.zeroDelta();
    delta.team = team;
    transmit(sky::ClientPacket::ReqPlayerDelta(delta));
  }
}

/**
 * MultiplayerView.
 */

MultiplayerView::MultiplayerView(
    sky::ArenaMode target,
    ClientShared &shared,
    MultiplayerCore &mShared) :
    ClientComponent(shared),
    ui::Control(shared.appState),
    mShared(mShared),
    conn(mShared.conn.get()),
    target(target) { }

