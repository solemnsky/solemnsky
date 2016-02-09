#include "multiplayerclient.h"
#include "util/methods.h"

MultiplayerClient::MultiplayerClient(ClientShared &state,
                                     const sf::IpAddress &serverAddress,
                                     const unsigned short serverPort,
                                     const unsigned short clientPort) :
    Game(state, "multiplayer"),

    quitButton({100, 50}, "quit tutorial"),
    chatEntry({20, 850}, "[enter] to chat"),
    messageLog({20, 840}),

    serverAddress(serverAddress),
    serverPort(serverPort),
    clientPort(clientPort),

    triedConnection(false),
    connected(false),
    telegraph(clientPort, sky::pk::clientPacketPack, sky::pk::serverPacketPack),
    pingCooldown(5),
    sky({}),
    renderSystem(&sky) {
  sky.linkSystem(&renderSystem);
}

void MultiplayerClient::handleGamePacket(
    tg::Reception<sky::prot::ServerPacket> &&reception) {
  using namespace sky::prot;
  switch (reception.value.type) {
    case ServerPacket::Type::Pong: {
      appLog(LogType::Info, "received pong from server!");
      break;
    }
    case ServerPacket::Type::Message: {
      appLog(LogType::Info, "received message from server: " +
          *reception.value.stringData);
      messageLog.pushEntry(std::string(*reception.value.stringData));
      break;
    }
    default:
      break;
  }
}

void MultiplayerClient::transmitServer(const sky::prot::ClientPacket &packet) {
  telegraph.transmit(packet, serverAddress, serverPort);
}


void MultiplayerClient::onLooseFocus() {
  quitButton.reset();
  chatEntry.reset();
}

void MultiplayerClient::onFocus() {

}

void MultiplayerClient::tick(float delta) {
  sky.tick(delta);

  quitButton.tick(delta);
  chatEntry.tick(delta);
  messageLog.tick(delta);

  using namespace sky::prot;

  if (!triedConnection) {
    transmitServer(
        ClientReqConnection(shared.settings.preferredNickname, clientPort));
    triedConnection = true;
    appLog(LogType::Debug, "sent connection request...");
  }

  if (!connected) {
    telegraph.receive([&](tg::Reception<ServerPacket> &&reception) {
      switch (reception.value.type) {
        case ServerPacket::Type::AcceptConnection: {
          arena = *reception.value.arena;
          myPID = *reception.value.pid;
          appLog(LogType::Info, "connected to server! MotD is: " + arena.motd);
          break;
        }
        default:
          break;
      }
    });
  } else {
    telegraph.receive([&](tg::Reception<ServerPacket> &&reception) {
      handleGamePacket(std::move(reception));
    });
  }

  if (pingCooldown.cool(delta)) {
    transmitServer(ClientPing());
    appLog(LogType::Info, "sending ping");
    pingCooldown.reset();
  }
}

void MultiplayerClient::render(ui::Frame &f) {
  renderSystem.render(f, {});

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


