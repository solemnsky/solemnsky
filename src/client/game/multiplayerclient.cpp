#include "multiplayerclient.h"
#include "util/methods.h"
#include "sky/protocol.h"

MultiplayerClient::MultiplayerClient(ClientState *state) :
    Game(state),
    quitButton({100, 50}, "quit tutorial", {}),
    telegraph(4243),
    pingCooldown(1) { }

void MultiplayerClient::tick(float delta) {
  quitButton.tick(delta);

  using sky::pk::serverMessagePack;
  using sky::pk::clientMessagePack;
  using namespace sky::prot;

  ServerPacket serverPacket;
  telegraph.receive([&](tg::Reception &&reception) {
    tg::unpackInto(serverMessagePack, reception.packet, serverPacket);
    switch (serverPacket.type) {
      case ServerPacket::Type::Pong: {
        appLog(LogType::Info, "received pong from server!");
        break;
      }
      case ServerPacket::Type::MotD: {
        appLog(LogType::Info, "received MotD from server: " +
                              *serverPacket.stringData);
        break;
      }
      case ServerPacket::Type::Message: {
        appLog(LogType::Info, "received message from server: " +
                              *serverPacket.stringData);
      }
    }
  });

  if (pingCooldown.cool(delta)) {
    tg::packInto(clientMessagePack, ClientPing(), buffer);
    telegraph.transmit(tg::Transmission(buffer, "localhost", 4242,
                                        tg::Strategy()));
    appLog(LogType::Info, "sending ping: " + buffer.dump());
    pingCooldown.reset();
  }
}

void MultiplayerClient::render(ui::Frame &f) {
  quitButton.render(f);
}

void MultiplayerClient::handle(const sf::Event &event) {
  quitButton.handle(event);
}

void MultiplayerClient::signalRead() {
  if (!quitButton.clickSignal.empty()) { concluded = true; }
}

void MultiplayerClient::signalClear() {
  quitButton.signalClear();
}
