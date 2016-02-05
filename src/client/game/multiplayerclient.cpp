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

  telegraph.receive([&](tg::Reception &&reception) {
    sky::ServerMessage message{
        tg::unpack(serverMessagePack, reception.packet)};
    switch (message.type) {
      case sky::ServerMessage::Type::Pong: {
        appLog(LogType::Info, "received pong from server!");
        break;
      }
      case sky::ServerMessage::Type::MotD: {
        appLog(LogType::Info, "received MotD from server: " + *message.motd);
        break;
      }
    }
  });

  if (pingCooldown.cool(delta)) {
    sky::ClientMessage message;
    message.joke = "no joke for you";
    message.type = sky::ClientMessage::Type::Ping;
    tg::packInto(clientMessagePack, message, buffer);
    telegraph.transmit(tg::Transmission(buffer, "localhost", 4242,
                                        tg::Strategy()));
    appLog(LogType::Info, "sending ping:");
    buffer.dump();
    tg::unpackInto(clientMessagePack, buffer, message);
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
