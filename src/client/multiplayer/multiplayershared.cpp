#include "multiplayershared.h"

/**
 * MultiplayerConnection.
 */
MultiplayerConnection::MultiplayerConnection(
    const std::string &serverHostname,
    const unsigned short serverPort) :
    server(nullptr),
    telegraph(sky::ServerPacketPack(), sky::ClientPacketPack()),
    askedConnection(true), disconnecting(false),
    disconnectTimeout(5),
    host(tg::HostType::Client),
    server(nullptr),
    disconnected(false),
    myPlayer(nullptr) {
  host.connect(serverHostname, serverPort);
}

void MultiplayerConnection::transmit(const sky::ClientPacket &packet) {
  if (server) telegraph.transmit(host, server, packet);
}

bool MultiplayerConnection::processPacket(const sky::ServerPacket &packet) {
  using namespace sky;

  if (!myPlayer) {
    // waiting for the arena connection request to be accepted
    if (packet.type == ServerPacket::Type::AckJoin) {
      if (!(packet.arenaInitializer and packet.pid))
        return false;

      shared.arena.emplace();
      if (!arena->applyInitializer(*packet.arenaInitializer)) {
        arena.reset();
        return false; // initializer didn't work
      }

      myPlayer = arena->getPlayer(*packet.pid);
      if (!myPlayer) {
        arena.reset();
        return false; // pid invalid
      }

      setUI(arena->mode);
      if (arena->mode == sky::ArenaMode::Game) {
        renderSystem.emplace(&*arena->sky);
      }

      appLog("Joined arena!", LogOrigin::Client);
      return true;
    }
    return false; // we're only interested in AckJoins until we're connected
  }

  // we're in the arena
  switch (packet.type) {
    case ServerPacket::Type::Pong: {
      return true; // received a pong from the server
    }

    case ServerPacket::Type::Message: {
      if (!packet.message) return false;
      switch (packet.message->type) {
        case ServerMessage::Type::Chat: {
          if (!packet.message->from) return false;
          messageLog.pushEntry("[chat]" + *packet.message->from +
              ": " + packet.message->contents);
          break;
        }
        case ServerMessage::Type::Broadcast: {
          messageLog.pushEntry(packet.message->contents);
          break;
        }
      }
      return true; // server sent us a Message
    }

    case ServerPacket::Type::NoteArenaDelta: {
      if (!packet.arenaDelta) return false;
      if (!arena->applyDelta(*packet.arenaDelta)) return false;

      if (packet.arenaDelta->type == sky::ArenaDelta::Type::Mode) {
        // we want to listen for changes in the mode
        setUI(arena->mode);
        if (arena->mode == sky::ArenaMode::Game)
          renderSystem.emplace(&*arena->sky);
      }

      return true; // server sent us a NoteArenaDelta
    }

    case ServerPacket::Type::NoteSkyDelta: {
      if (!packet.skyDelta) return false;
      if (arena->sky) arena->sky->applyDelta(*packet.skyDelta);
      return true; // server sent us a NoteSkyDelta
    }

    default:
      break;
  }

  return false; // invalid packet type, considering we're in the arena
}

optional<sky::ServerPacket> MultiplayerConnection::poll(const float delta) {
  event = host.poll();
  if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
    server = nullptr;
    appLog("Disconnected from server!", LogOrigin::Client);
    disconnected = true;
    return {};
  }

  if (disconnecting) {
    if (disconnectTimeout.cool(delta)) {
      appLog("Disconnecting from unresponsive server!", LogOrigin::Client);
      disconnected = true;;
    }
    return {};
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
      return {};
    }

    if (event.type == ENET_EVENT_TYPE_RECEIVE) {
      if (const auto reception = telegraph.receive(event.packet)) {
        if (processPacket(*reception)) return;
      }
      appLog("Received malformed packet from server!", LogOrigin::Client);
    }
  }
}

void MultiplayerConnection::disconnect() {

}


/**
 * MultiplayerView.
 */

MultiplayerView::Style::Style() :
    scoreOverlayDims{1330, 630},
    chatPos(20, 850),
    messageLogPos(20, 840),
    readyButtonPos(lobbyChatWidth + 10, lobbyTopMargin),
    scoreOverlayTopMargin(100),
    lobbyPlayersOffset(1250),
    lobbyTopMargin(205),
    lobbyChatWidth(1250),
    gameChatWidth(500),
    lobbyFontSize(40),
    playerSpecColor(255, 255, 255),
    playerJoinedColor(0, 255, 0),
    readyButtonActiveDesc("ready!"),
    readyButtonDeactiveDesc("cancel") { }

MultiplayerView::MultiplayerView(
    ClientShared &shared,
    MultiplayerConnection &mShared) :
    shared(shared),
    connection(mShared) { }

