#include "multiplayershared.h"
#include "client/elements/style.h"
#include "sky/event.h"

/**
 * MultiplayerLogger.
 */

void MultiplayerLogger::onEvent(const sky::ArenaEvent &event) {
  connection.logArenaEvent(event);
}

MultiplayerLogger::MultiplayerLogger(sky::Arena &arena,
                                     MultiplayerShared &connection) :
    sky::ArenaLogger(arena), connection(connection) { }

/**
 * MultiplayerConnection.
 */

void MultiplayerShared::processPacket(const sky::ServerPacket &packet) {
  using namespace sky;

  if (!player) {
    // waiting for the arena connection request to be accepted
    if (packet.type == ServerPacket::Type::Init) {
      initializeArena(
          packet.pid.get(), packet.arenaInit.get(), packet.skyInit.get());
      appLog("Joined arena!", LogOrigin::Client);

      logEvent(ClientEvent::Connect(
          "(haven't implemented server names lol)",
          tg::printAddress(host.peers[0]->address)));
    }
    return;
  }

  // we're in the arena
  switch (packet.type) {
    case ServerPacket::Type::Pong: {
      break;
    }

    case ServerPacket::Type::DeltaArena: {
      arena->applyDelta(packet.arenaDelta.get());
      break;
    }

    case ServerPacket::Type::DeltaSky: {
      break;
    }

    case ServerPacket::Type::Chat: {
      if (sky::Player *player = arena->getPlayer(
          packet.pid.get())) {
        logEvent(ClientEvent::Chat(
            player->nickname, packet.stringData.get()));
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

void MultiplayerShared::logEvent(const ClientEvent &event) {
  StringPrinter p;
  event.print(p);
  appLog(p.getString(), LogOrigin::Client);
  eventLog.push_back(event);
}

void MultiplayerShared::logArenaEvent(const sky::ArenaEvent &event) {
  StringPrinter p;
  event.print(p);
  appLog(p.getString(), LogOrigin::Engine);
  eventLog.push_back(ClientEvent::Event(event));
}

MultiplayerShared::MultiplayerShared(
    ClientShared &shared,
    const std::string &serverHostname,
    const unsigned short serverPort) :
    shared(shared),
    server(nullptr),
    telegraph(),
    askedConnection(false),
    disconnectTimeout(5),
    host(tg::HostType::Client),
    disconnected(false), disconnecting(false),
    player(nullptr), plane(nullptr) {
  host.connect(serverHostname, serverPort);
}

void MultiplayerShared::initializeArena(
    const PID pid,
    const sky::ArenaInitializer &arenaInit,
    const sky::SkyInitializer &skyInit) {
  arena.emplace(arenaInit);
  sky.emplace(arena.get(), skyInit);
  skyRender.emplace(arena.get(), sky.get(), shared.settings.enableDebug);
  logger.emplace(arena.get(), *this);
  player = arena->getPlayer(pid);
  plane = &sky->getPlane(*player);
}

void MultiplayerShared::transmit(const sky::ClientPacket &packet) {
  if (server) telegraph.transmit(host, server, packet);
}

void MultiplayerShared::poll(const float delta) {
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
      appLog("Connected to server!", LogOrigin::Client);
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

void MultiplayerShared::disconnect() {
  if (server) {
    host.disconnect(server);
    disconnecting = true;
    disconnectTimeout.reset();
  } else {
    disconnected = true;
  }
}

void MultiplayerShared::onChangeSettings(const SettingsDelta &settings) {
  if (skyRender) {
    if (settings.enableDebug)
      skyRender->enableDebug = settings.enableDebug.get();
  }
}

void MultiplayerShared::chat(const std::string &message) {
  transmit(sky::ClientPacket::Chat(message));
}

void MultiplayerShared::rcon(const std::string &command) {
  logEvent(ClientEvent::RConCommand(command));
  transmit(sky::ClientPacket::RCon(command));
}

void MultiplayerShared::handleChatInput(const std::string &input) {
  if (input.size() >= 1) {
    if (input.size() > 1 and input[0] == '/') {
      rcon(input.substr(1));
    } else {
      chat(input);
    }
  }
}

void MultiplayerShared::requestTeamChange(const sky::Team team) {
  if (player) {
    sky::PlayerDelta delta = player->zeroDelta();
    delta.team = team;
    transmit(sky::ClientPacket::ReqPlayerDelta(delta));
  }
}

void MultiplayerShared::drawEventLog(ui::Frame &f, const float cutoff) {
  f.drawText(
      style.multi.messageLogPos, [&](ui::TextFrame &tf) {
        for (auto iter = eventLog.rbegin();
             iter < eventLog.rend(); iter++) {
          ClientEvent(*iter).print(tf);
          tf.breakLine();
          if (tf.drawOffset.y < -cutoff) break;
        }
      }, sf::Color::White, style.multi.messageLogText);
}

/**
 * MultiplayerView.
 */

MultiplayerView::MultiplayerView(
    sky::ArenaMode target,
    ClientShared &shared,
    MultiplayerShared &mShared) :
    target(target),
    shared(shared),
    mShared(mShared) { }

