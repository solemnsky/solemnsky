#include "multiplayerclient.h"
#include "util/methods.h"

using sky::pk::serverPacketPack;
using sky::pk::clientPacketPack;

MultiplayerClient::MultiplayerClient(ClientShared &state) :
    quitButton({100, 50}, "quit tutorial"),
    chatEntry({20, 850}, "[enter] to chat"),
    messageLog({20, 840}),
    sky({}),
    renderSystem(&sky),
    Game(state, "multiplayer"),
    telegraph(4243, clientPacketPack, serverPacketPack),
    pingCooldown(1) {
  sky.linkSystem(&renderSystem);
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
        messageLog.pushEntry(std::string(*reception.value.stringData));
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
    telegraph.transmit(
        sky::prot::ClientChat(std::string(*chatEntry.inputSignal)),
        "localhost", 4242);
}

void MultiplayerClient::signalClear() {
  quitButton.signalClear();
  chatEntry.signalClear();
}

