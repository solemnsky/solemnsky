#include "multiplayerclient.h"
#include "util/methods.h"

using sky::pk::serverPacketPack;
using sky::pk::clientPacketPack;

MultiplayerClient::MultiplayerClient(ClientShared &state) :
    Game(state, "multiplayer"),
    quitButton({100, 50}, "quit tutorial"),
    chatEntry({500, 500}, "chat"),
    telegraph(4243, clientPacketPack, serverPacketPack),
    pingCooldown(1) { }

void MultiplayerClient::tick(float delta) {
  quitButton.tick(delta);
  chatEntry.tick(delta);

  using namespace sky::prot;

  telegraph.receive([&](tg::Reception<ServerPacket> &&reception) {
    switch (reception.value.type) {
      case ServerPacket::Type::Pong: {
        appLog(LogType::Info, "received pong from server!");
        break;
      }
      case ServerPacket::Type::MotD: {
        appLog(LogType::Info, "received MotD from server: " +
                              *reception.value.stringData);
        break;
      }
      case ServerPacket::Type::Message: {
        appLog(LogType::Info, "received message from server: " +
                              *reception.value.stringData);
      }
    }
  });

  if (pingCooldown.cool(delta)) {
    telegraph.transmit(ClientPing(), "localhost", 4242);
    appLog(LogType::Info, "sending ping");
    pingCooldown.reset();
  }
}

void MultiplayerClient::render(ui::Frame &f) {
  quitButton.render(f);
  chatEntry.render(f);
}

bool MultiplayerClient::handle(const sf::Event &event) {
  if (chatEntry.handle(event)) return true;
  if (quitButton.handle(event)) return true;
  if (event.type == sf::Event::EventType::KeyPressed) {
    if (event.key.code == sf::Keyboard::Return)
      telegraph.transmit(
          sky::prot::ClientChat(std::string("hey there server!")),
          "localhost", 4242);
    return true;
  }
  return false;
}

void MultiplayerClient::signalRead() {
  if (quitButton.clickSignal) concluded = true;
}

void MultiplayerClient::signalClear() {
  quitButton.signalClear();
}
