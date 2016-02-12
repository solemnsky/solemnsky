#include "multiplayerclient.h"
#include "util/methods.h"

MultiplayerClient::MultiplayerClient(ClientShared &state,
                                     const std::string &serverHostname,
                                     const unsigned short serverPort) :
    Game(state, "multiplayer"),

    quitButton({100, 50}, "quit tutorial"),
    chatEntry({20, 850}, "[enter] to chat"),
    messageLog({20, 840}),

    host(tg::HostType::Client),
    server(nullptr),
    telegraph(sky::pk::serverPacketPack, sky::pk::clientPacketPack),
    pingCooldown(5),
    connected(false) {
  host.connect(serverHostname, serverPort);
}
/**
 * Networking submethods.
 */

void MultiplayerClient::transmitServer(const sky::prot::ClientPacket &packet) {
  if (server and connected) telegraph.transmit(host, server, packet);
}

void MultiplayerClient::handleNetwork(const ENetEvent &event) {

}

/**
 * Game interface.
 */

void MultiplayerClient::onLooseFocus() {
  quitButton.reset();
  chatEntry.reset();
}

void MultiplayerClient::onFocus() {

}

void MultiplayerClient::onExit() {
  // we could try to disconnect from the server gracefully here
  concluded = true;
}

/**
 * Control interface.
 */

void MultiplayerClient::tick(float delta) {
  quitButton.tick(delta);
  chatEntry.tick(delta);
  messageLog.tick(delta);

  using namespace sky::prot;

  if (!triedConnection) {
    transmitServer(ClientReqConnection(shared.settings.preferredNickname));
    appLog("Connecting to server...", LogOrigin::Client);
  }

  event = host.poll();
  if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
    server = nullptr;
    appLog("Disconnected from server!", LogOrigin::Client);
    onExit();
  }

  if (!server) {
    // still trying to connect to the server...
    if (event.type == ENET_EVENT_TYPE_CONNECT) {
      server = event.peer;
      appLog("Found server...", LogOrigin::Client);
    }
  } else {
    if (!connected) {
      if (event.type == ENET_EVENT_TYPE_RECEIVE) {
        const ServerPacket &packet = telegraph.receive(event.packet);
        if (packet.type == ServerPacket::Type::AcceptConnection) {
          arena = *packet.arena;
          myRecord = arena.getPlayer(*packet.pid);
          connected = true;
          appLog("Connected to server!", LogOrigin::Client);
        }
      }
    } else {
      handleNetwork(event);
    }
  }
}

void MultiplayerClient::render(ui::Frame &f) {
  quitButton.render(f);
  chatEntry.render(f);
  messageLog.render(f);
}

bool MultiplayerClient::handle(const sf::Event &event) {
  if (chatEntry.handle(event)) return true;
  if (quitButton.handle(event)) return true;
  if (event.type == sf::Event::EventType::KeyPressed) {
    if (event.key.code == sf::Keyboard::Return) {
      chatEntry.focus();
    }
  }
  return messageLog.handle(event);
}

void MultiplayerClient::signalRead() {
  if (quitButton.clickSignal) concluded = true;
  if (chatEntry.inputSignal)
    transmitServer(
        sky::prot::ClientChat(std::string(*chatEntry.inputSignal)));
}

void MultiplayerClient::signalClear() {
  quitButton.signalClear();
  chatEntry.signalClear();
}


