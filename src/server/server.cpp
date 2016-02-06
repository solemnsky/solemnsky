#include "server.h"

#include "telegraph/telegraph.h"
#include "sky/delta.h"
#include "sky/protocol.h"
#include <iostream>
#include "util/methods.h"

int main() {
  using sky::pk::clientMessagePack;
  using sky::pk::serverMessagePack;
  using namespace sky::prot;

  tg::Telegraph telegraph(4242);

  ClientPacket clientPacket;
  tg::Packet buffer;

  bool running = true;
  while (running) {
    telegraph.receive([&](tg::Reception &&reception) {
      appLog(LogType::Debug, "message received: " + reception.packet.dump());
      tg::unpackInto(clientMessagePack, reception.packet, clientPacket);
      switch (clientPacket.type) {
        case ClientPacket::Type::Ping: {
          appLog(LogType::Info, "message is a ping request");
          appLog(LogType::Info, "responding with pong");

          tg::packInto(serverMessagePack, ServerPong(), buffer);
          telegraph.transmit(tg::Transmission(
              buffer, reception.address, 4243, tg::Strategy()));
          break;
        }
        case ClientPacket::Type::MotD: {
          appLog(LogType::Info, "message is a MotD request");
          appLog(LogType::Info, "responding with MotD");

          tg::packInto(
              serverMessagePack,
              ServerMotD(
                  std::string("Wheeeee I'm in a UDP packet look at me!")),
              buffer);
          telegraph.transmit(tg::Transmission(
              buffer, reception.address, 4243, tg::Strategy()));
          break;
        }
        case ClientPacket::Type::Chat: {

        }
      }
    });
  }
}
